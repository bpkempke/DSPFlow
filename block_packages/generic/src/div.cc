#include <div.h>
#include <string.h>
#include <cstdlib>

div::div(flowBlockDescription in_desc) : flowBlock(in_desc){
	input_prim_type = getInputPrimitiveType();
	if(input_prim_type == PRIM_VOID) //TODO: THROW SOMETHING SENSIBLE!
		throw 0;

	//We'll set up a function pointer since we don't want to waste time doing a jump table or whatever every time we need to process data...
	if(input_prim_type == PRIM_INT8)
		div_fptr = &div::div_int8;
	else if(input_prim_type == PRIM_INT32)
		div_fptr = &div::div_int32;
	else if(input_prim_type == PRIM_FLOAT)
		div_fptr = &div::div_float;
	else if(input_prim_type == PRIM_DOUBLE)
		div_fptr = &div::div_double;

	//Start out with a modestly-sized array for using for temp storage
	dived_data_size = 0;
}

div::~div(){
	free(dived_data);
}

void div::process(){
	//First figure out how much data is ready to div together
	int num_input_elements = getMinInputPipeUsage();

	//For now, just do a dumb divition based on the primitive type of the input pipes
	(*this.*div_fptr)(num_input_elements);
}

//TODO: put in some intrinsics for processors which have SSE2/SSE3/SSE4 support (sounds like a big pain...

void div::div_int8(int num_elements){
	//Check to see if we have enough space
	if(num_elements > dived_data_size){
		dived_data_size = num_elements*2;
		free(dived_data);
		dived_data = malloc(dived_data_size);
	}

	char *dived_data_ptr = (char*)dived_data;
	
	//Start out with the contents of the first input buffer
	memcpy(dived_data_ptr, block_info.inputs[0]->consumePrimitiveData(num_elements), num_elements);

	//Now div stuff together into the scratchspace
	for(int ii=0; ii < block_info.inputs.size(); ii++){
		char *current_input_buffer = (char*)block_info.inputs[ii]->consumePrimitiveData(num_elements);
		for(int jj=0; jj < num_elements; jj++){
			dived_data_ptr[jj] /= current_input_buffer[jj];
		}
	}
}

void div::div_int32(int num_elements){
	//Check to see if we have enough space
	if(num_elements*4 > dived_data_size){
		dived_data_size = num_elements*2*4;
		free(dived_data);
		dived_data = malloc(dived_data_size*sizeof(int32_t));
	}
	
	int32_t *dived_data_ptr = (int32_t*)dived_data;
	
	//Start out with the contents of the first input buffer
	memcpy(dived_data_ptr, block_info.inputs[0]->consumePrimitiveData(num_elements), num_elements*4);

	//Now div stuff together into the scratchspace
	for(int ii=0; ii < block_info.inputs.size(); ii++){
		int32_t *current_input_buffer = (int32_t*)block_info.inputs[ii]->consumePrimitiveData(num_elements);
		for(int jj=0; jj < num_elements; jj++){
			dived_data_ptr[jj] /= current_input_buffer[jj];
		}
	}
}

void div::div_float(int num_elements){
	//Check to see if we have enough space
	if(num_elements*4 > dived_data_size){
		dived_data_size = num_elements*2*4;
		free(dived_data);
		dived_data = malloc(dived_data_size*sizeof(float));
	}
	
	float *dived_data_ptr = (float*)dived_data;
	
	//Start out with the contents of the first input buffer
	memcpy(dived_data, block_info.inputs[0]->consumePrimitiveData(num_elements), num_elements*4);

	//Now div stuff together into the scratchspace
	for(int ii=0; ii < block_info.inputs.size(); ii++){
		float *current_input_buffer = (float*)block_info.inputs[ii]->consumePrimitiveData(num_elements);
		for(int jj=0; jj < num_elements; jj++){
			dived_data_ptr[jj] /= current_input_buffer[jj];
		}
	}
}

void div::div_double(int num_elements){
	//Check to see if we have enough space
	if(num_elements*8 > dived_data_size){
		dived_data_size = num_elements*2*8;
		free(dived_data);
		dived_data = malloc(dived_data_size*sizeof(double));
	}
	
	double *dived_data_ptr = (double*)dived_data;
	
	//Start out with the contents of the first input buffer
	memcpy(dived_data_ptr, block_info.inputs[0]->consumePrimitiveData(num_elements), num_elements*8);

	//Now div stuff together into the scratchspace
	for(int ii=0; ii < block_info.inputs.size(); ii++){
		double *current_input_buffer = (double*)block_info.inputs[ii]->consumePrimitiveData(num_elements);
		for(int jj=0; jj < num_elements; jj++){
			dived_data_ptr[jj] /= current_input_buffer[jj];
		}
	}
}
