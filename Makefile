##
# $Id: Makefile 1249 2014-02-22 20:21:40Z ales.bardorfer $
#
# (c) Red Pitaya  http://www.redpitaya.com
#
# Trigger
# Trigger project source file. 
#
# make CROSS_COMPILE=arm-linux-gnueabi- clean all
# on PC and move to Red Pitaya:
# scp trig root@172.16.255.27:/tmp
#
# This project file is written for GNU/Make software. For more details please 
# visit: http://www.gnu.org/software/make/manual/make.html
# GNU Compiler Collection (GCC) tools are used for the compilation and linkage. 
# For the details about the usage and building please visit:
# http://gcc.gnu.org/onlinedocs/gcc/
#

# Versioning system
VERSION ?= 0.00-0000
REVISION ?= devbuild


# old def de SRCS et OBJS
# List of compiled object files (not yet linked to executable)
#OBJS = trigger.o fpga_osc.o# main_osc.o worker.o
# List of raw source files (all object files, renamed from .o to .c)
#SRCS = $(subst .o,.c, $(OBJS)))

# new def
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

# Executable name
TARGET=trig

# GCC compiling & linking flags
CFLAGS=-g -std=gnu99 -Wall -Werror
CFLAGS += -DVERSION=$(VERSION) -DREVISION=$(REVISION)
CFLAGS += -I/opt/redpitaya/include  -L/opt/redpitaya/lib



# Red Pitaya common SW directory
SHARED=../../shared/

# Additional libraries which needs to be dynamically linked to the executable
# -lm - System math library (used by cos(), sin(), sqrt(), ... functions)
LIBS=-lm -lpthread -lrp -l:librp.a



# Main GCC executable (used for compiling and linking)
CC=$(CROSS_COMPILE)gcc
# Installation directory
INSTALL_DIR ?= .

# Makefile is composed of so called 'targets'. They give basic structure what 
# needs to be execued during various stages of the building/removing/installing
# of software package.
# Simple Makefile targets have the following structure:
# <name>: <dependencies>
#	<command1>
#       <command2>
#       ...
# The target <name> is completed in the following order:
#   - list od <dependencies> finished
#   - all <commands> in the body of targets are executed succsesfully

# Main Makefile target 'all' - it iterates over all targets listed in $(TARGET)
# variable.
all: $(TARGET)

# Target with compilation rules to compile object from source files.
# It applies to all files ending with .o. During partial building only new object
# files are created for the source files (.c) which have newer timestamp then 
# objects (.o) files.
%.o: %.c version.h
	$(CC) -c $(CFLAGS) $< -o $@

# Makefile target with rules how to link executable for each target from $(TARGET)
# list.
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# Version header for traceability
version.h:
	cp $(SHARED)/include/redpitaya/version.h . 

# Clean target - when called it cleans all object files and executables.
clean:
	rm -f $(TARGET) *.o

# Install target - creates 'bin/' sub-directory in $(INSTALL_DIR) and copies all
# executables to that location.
install:
	mkdir -p $(INSTALL_DIR)/bin
	cp $(TARGET) $(INSTALL_DIR)/bin
	mkdir -p $(INSTALL_DIR)/src/utils/$(TARGET)
	-rm -f $(TARGET) *.o
	cp -r * $(INSTALL_DIR)/src/utils/$(TARGET)/
	-rm `find $(INSTALL_DIR)/src/tools/$(TARGET)/ -iname .svn` -rf
