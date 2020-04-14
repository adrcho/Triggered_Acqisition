/**
 * $Id: trigger.c peter.ferjancic 2014/11/17 $
 *
 * @brief Red Pitaya triggered acquisition of multiple traces
 *
 * (c) Red Pitaya  http://www.redpitaya.com
 *
 * This part of code is written in C programming language.
 * Please visit http://en.wikipedia.org/wiki/C_(programming_language)
 * for more details on the language used herein.
 */

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stddef.h>
#include <sys/param.h>
#include <stdint.h>
#include "fpga_osc.h"


#include "redpitaya/rp.h"


int unsigned led0=0;
int unsigned led1=1;
int unsigned led2=2;




//Buffer depth 

const int BUF = 16*1024;
const int decimation = 8;    // decimation: [1;8;64;1024;8192;65536]
const int N= 1562;			// desired length of trace (1,..., 16383) for 100us recording 
	// decimation: [1;8;64;1024;8192;65536]

int main(void) 
{
	clock_t start_t, loop_t, loop_t1, loop_t2, end_t, total_t;
	printf("%i ",N);
	// initialization
	int start = osc_fpga_init(); 
	if(start) {
	    printf("osc_fpga_init didn't work, retval = %d",start);
	    return -1;
	}
	FILE * fp;
	fp = fopen ("file.txt", "w+"); // open the file output is stored
	int * cha_signal; 
	int * chb_signal;

	// set acquisition parameters
	osc_fpga_set_trigger_delay(N);
	g_osc_fpga_reg_mem->data_dec = decimation;

	// put sampling engine into reset
	osc_fpga_reset();

	// define initial parameters
	int trace_counts; 		// counts how many traces were sampled in for loop
	int trig_ptr; 			// trigger pointer shows memory adress where trigger was met
	int trig_test;			// trigger test checks if writing the trace has completed yet
	int trigger_voltage= 0; // enter trigger voltage in [V] as parameter [1V...~600 RP units]
	g_osc_fpga_reg_mem->cha_thr = osc_fpga_cnv_v_to_cnt(trigger_voltage); //sets trigger voltage
	
	
	
	
	// Initialization of API
	 if (rp_Init() != RP_OK) {
        fprintf(stderr, "Red Pitaya API init failed!\n");
        return EXIT_FAILURE;
    	}
	
	rp_DpinSetState(led0, RP_LOW);
	rp_DpinSetState(led1, RP_LOW);
	rp_DpinSetState(led2, RP_LOW);
	
	
	
	
	// define initial parameters for trigger signal
	int trig_sig_freq=10000;     // generation of a trigger signal
	int trig_sig_amp=1;

	/******************************/
	/** Set emission for trigger **/
	/******************************/
	
	/* Generating frequency */
	rp_GenFreq(RP_CH_1,trig_sig_freq);
	/* Generating amplitude */
	rp_GenAmp(RP_CH_1, trig_sig_amp);
	/* Generating wave form */
	rp_GenWaveform(RP_CH_1, RP_WAVEFORM_SINE);
	/* Enable channel */
	rp_GenOutEnable(RP_CH_1);
	
	rp_DpinSetState(led0, RP_HIGH);
	
	// define initial parameters for test signal
	int test_sig_freq=1000;     // generation of a trigger signal
	int test_sig_amp=0.5;
	
	/**********************************/
	/** Set emission for test signal **/
	/**********************************/
	
	/* Generating frequency */
	rp_GenFreq(RP_CH_2,test_sig_freq);
	/* Generating amplitude */
	rp_GenAmp(RP_CH_2, test_sig_amp);
	/* Generating wave form */
	rp_GenWaveform(RP_CH_2, RP_WAVEFORM_SINE);
	/* Enable channel */
	rp_GenOutEnable(RP_CH_2);
	
	rp_DpinSetState(led1, RP_HIGH);
	
	
	start_t= clock();
	/***************************/
	/** MAIN ACQUISITION LOOP **/
	/***************************/
	for (trace_counts=0; trace_counts<100; trace_counts++)
	{
		loop_t1=clock();
		/*Set trigger, begin acquisition when condition is met*/
		osc_fpga_arm_trigger(); //start acquiring, incrementing write pointer
		osc_fpga_set_trigger(0x2); // where do you want your triggering from?
	 	/*    0 - end of acquisition/no acquisition
	 	*     1 - trig immediately
		*     2 - ChA positive edge 
		*     3 - ChA negative edge
		*     4 - ChB positive edge 
		*     5 - ChB negative edge
		*     6 - External trigger 0
		*     7 - External trigger 1*/
			// Trigger always changes to 0 after acquisition is completed, write pointer stops incrementing
			//->fpga.osc.h l66


		    /*Wait for the acquisition to finish = trigger is set to 0*/
		trig_test=(g_osc_fpga_reg_mem->trig_source); // it gets the above trigger value
		// if acquistion is not yet completed it should return the number you set above and 0 otherwise
		while (trig_test!=0) // with this loop the program waits until the acquistion is completed before cont.
		{
			trig_test=(g_osc_fpga_reg_mem->trig_source);
		}
		//->fpga_osc.c l366
		rp_DpinSetState(led2, RP_HIGH);

	    trig_ptr = g_osc_fpga_reg_mem->wr_ptr_trigger; // get pointer to mem. adress where trigger was met
	    //->fpga_osc.c l283
	    osc_fpga_get_sig_ptr(&cha_signal, &chb_signal); 
	    //->fpga_osc.c l378

		
		
		
		
	//----------------------------------------//
	//-------------Data processing-------_----//
	//-------------Negative nalues------------//
	//----------------------------------------//	
		/*now read N samples from the trigger pointer location.*/
	//    int i;
	//    int ptr;
	//    for (i=0; i < N; i++)
	//    {
	//	ptr = (trig_ptr+i)%BUF;

	//	if (cha_signal[ptr]>=8192) // properly display negative values fix
	//	{
	//		//printf("%d ",cha_signal[ptr]-16384);
	//		fprintf(fp, "%d ", cha_signal[ptr]-16384);
	//	}
	//	else
	//	{
	//		//printf("%d ",cha_signal[ptr]);
	//		fprintf(fp, "%d ", cha_signal[ptr]);
	//	}  
	//   }
	    	
		
		
		
		
	//----------------------------------------//
	//-----------No Data processing-----------//
	//-------------Negative nalues------------//
	//----------------------------------------//
		//gain 20% speed
		
	//    /*now read N samples from the trigger pointer location.*/
	//    int i;
	//    int ptr;
	//    for (i=0; i < N; i++)
	//    {
	//	ptr = (trig_ptr+i)%BUF;
	//	
	//	fprintf(fp, "%d ", cha_signal[ptr]);
 
	//    }
	
		
	//----------------------------------------//
	//-----------No Data processing-----------//
	//-------------Negative nalues------------//
	//----------------------------------------//
	//----------------------------------------//
	//--------(Single_values_collection-------//
	//----------------------------------------//
	// same speed for writing in a file.
			
	//    /*now read N samples from the trigger pointer location.*/
	//    int i;
	//    for (i=0; i < N; i++)
	//    {
	//	fprintf(fp, "%d ", cha_signal[(trig_ptr+i)%BUF]); 
	//    }
	    
		
		
		
		

	//----------------------------------------//
	//-----------No Data processing-----------//
	//-------------Negative nalues------------//
	//----------------------------------------//
	//----------------------------------------//
	//-------Multiple_values_collection-------//
	//----------------------------------------//
	// same speed for writing in a file.
			
	    /*now read N samples from the trigger pointer location.*/
	    int i;
	    int data[N];  // sure its an int?
	    printf("%p", data);
	    for (i=0; i < N; i+=1)
	    {
		memcpy(&data[i], &cha_signal[(trig_ptr+i)%BUF], 1*sizeof(int));
	    }
	    
	    //fprintf(fp, "%d ", data); 
		
		       
		       
		       
	    loop_t2=clock();
	    loop_t = (long double)(loop_t2 - loop_t1);
	    printf("Ellapsed time %ld clock cycles\n" ,loop_t);


	    rp_DpinSetState(led2, RP_HIGH);
	    printf("Sample %i recorded\n",trace_counts);
	    //printf("\n");
	    fprintf(fp, "\n");
		
		
		
	    
	}
	
	
	
	end_t=clock();
	// cleaning up all nice like mommy taught me
	fclose(fp);
 	osc_fpga_exit();
	total_t = (long double)(end_t - start_t);
	printf("Total ellapsed time %ld clock cycles\n" ,total_t);
	printf("%ld Clock cycles per sec\n" ,CLOCKS_PER_SEC);
	
	rp_GenOutDisable(RP_CH_1);
	rp_GenOutDisable(RP_CH_2);
	rp_DpinSetState(led0, RP_LOW);
	rp_DpinSetState(led1, RP_LOW);
	rp_DpinSetState(led2, RP_LOW);
	rp_Release();
	
	
	
	return 0;
}
