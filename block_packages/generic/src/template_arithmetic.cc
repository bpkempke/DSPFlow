#include<template_arithmetic.h>
#include <string.h>
#include<cstdlib>
#include <stdio.h>

template <class T, template<class T> class ArithmeticOp>
template_arithmetic::template_arithmetic(flowBlock *parent_flowblock){
	//Save the parent flowBlock element for later
	parent_block = parent_flowblock;

	//Start out with a modestly-sized array for using for temp storage
	data_size = 0;
}

template <class T, template<class T> class ArithmeticOp>
template_arithmetic::~template_arithmetic(){
	free(data);
}

template <class T, template<class T> class ArithmeticOp>
void template_arithmetic::process(){
	//First figure out how much data is ready to add together
	int num_elements = parent_block->getMinInputPipeUsage();

	//Check to see if we have enough space
	if(num_elements*sizeof(T) > data_size){
		data_size = num_elements*2*sizeof(T);
		free(data);
		data = malloc(data_size*sizeof(T));
	}
	
	T *data_ptr = (T*)data;
	
	//Start out with the contents of the first input buffer
	memcpy(data_ptr, block_info.inputs[0]->consumePrimitiveData(num_elements), num_elements*sizeof(T));

	//Now add stuff together into the scratchspace
	for(unsigned int ii=1; ii < block_info.inputs.size(); ii++){
		T *current_input_buffer = (T*)block_info.inputs[ii]->consumePrimitiveData(num_elements);
		for(int jj=0; jj < num_elements; jj++){
			data_ptr[jj] = ArithmeticOp<T>()(data_ptr[jj],current_input_buffer[jj]);
		}
	}
	block_info.outputs[0]->insertPrimitiveData(data_ptr, num_elements*sizeof(T));
}

