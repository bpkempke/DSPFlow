#
# DSPFlow Makefile
#
# Ben Kempke
#
# Shared libraries should be compiled as follows:
#  gcc -shared -nostartfiles -o bar bar.c
#
# Also make sure to install libdb in order to compile...:
#  sudo apt-get install libdb-dev

CC = gcc
CPPC = g++
THREAD_LIB = -pthread 
MISC_LIBS = -ldl
DSPFLOW_CORE = src/flowGraph.cc src/flowBlock.cc src/flowPipe.cc src/naclDL.cc
DSPFLOW_CORE_LIB = lib/flowGraph.h lib/flowBlock.h lib/flowPipe.h lib/naclDL.h
TINYXML = src/tinyxml2.cpp
INCLUDE = -Ilib

all: dspflow

dspflow: $(DSPFLOW_CORE) $(DSPFLOW_CORE_LIB) $(TINYXML)
	$(CPPC) $(THREAD_LIB) $(INCLUDE) $(TINYXML) $(DSPFLOW_CORE) $(DSPFLOW_CORE_LIB) -o ./bin/dspflow $(MISC_LIBS)

clean:
	rm -f ./bin/obj/*.o

