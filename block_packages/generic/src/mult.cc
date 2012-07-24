#include <mult.h>
#include <string.h>
#include <cstdlib>

mult::mult(flowBlockDescription in_desc) : flowBlock(in_desc){
	input_prim_type = getInputPrimitiveType();
	if(input_prim_type == PRIM_VOID) //TODO: THROW SOMETHING SENSIBLE!
		throw 0;

	//We'll set up a function pointer since we don't want to waste time doing a jump table or whatever every time we need to process data...
	if(input_prim_type == PRIM_INT8)
		mult_fptr = &mult::mult_int8;
	else if(input_prim_type == PRIM_INT32)
		mult_fptr = &mult::mult_int32;
	else if(input_prim_type == PRIM_FLOAT)
		mult_fptr = &mult::mult_float;
	else if(input_prim_type == PRIM_DOUBLE)
		mult_fptr = &mult::mult_double;

	//Start out with a modestly-sized array for using for temp storage
	multed_data_size = 0;
}

mult::~mult(){
	free(multed_data);
}

void mult::process(){
	//First figure out how much data is ready to mult together
	int num_input_elements = getMinInputPipeUsage();

	//For now, just do a dumb addition based on the primitive type of the input pipes
	(*this.*mult_fptr)(num_input_elements);
}

//TODO: put in some intrinsics for processors which have SSE2/SSE3/SSE4 support (sounds like a big pain...

void mult::mult_int8(int num_elements){
	//Check to see if we have enough space
	if(num_elements > multed_data_size){
		multed_data_size = num_elements*2;
		free(multed_data);
		multed_data = malloc(multed_data_size);
	}

	char *multed_data_ptr = (char*)multed_data;
	
	//Start out with the contents of the first input buffer
	memcpy(multed_data_ptr, block_info.inputs[0]->consumePrimitiveData(num_elements), num_elements);

	//Now mult stuff together into the scratchspace
	for(unsigned int ii=0; ii < block_info.inputs.size(); ii++){
		char *current_input_buffer = (char*)block_info.inputs[ii]->consumePrimitiveData(num_elements);
		for(int jj=0; jj < num_elements; jj++){
			multed_data_ptr[jj] *= current_input_buffer[jj];
		}
	}
}

void mult::mult_int32(int num_elements){
	//Check to see if we have enough space
	if(num_elements*4 > multed_data_size){
		multed_data_size = num_elements*2*4;
		free(multed_data);
		multed_data = malloc(multed_data_size*sizeof(int32_t));
	}
	
	int32_t *multed_data_ptr = (int32_t*)multed_data;
	
	//Start out with the contents of the first input buffer
	memcpy(multed_data_ptr, block_info.inputs[0]->consumePrimitiveData(num_elements), num_elements*4);

	//Now mult stuff together into the scratchspace
	for(unsigned int ii=0; ii < block_info.inputs.size(); ii++){
		int32_t *current_input_buffer = (int32_t*)block_info.inputs[ii]->consumePrimitiveData(num_elements);
		for(int jj=0; jj < num_elements; jj++){
			multed_data_ptr[jj] *= current_input_buffer[jj];
		}
	}
}

void mult::mult_float(int num_elements){
	//Check to see if we have enough space
	if(num_elements*4 > multed_data_size){
		multed_data_size = num_elements*2*4;
		free(multed_data);
		multed_data = malloc(multed_data_size*sizeof(float));
	}
	
	float *multed_data_ptr = (float*)multed_data;
	
	//Start out with the contents of the first input buffer
	memcpy(multed_data, block_info.inputs[0]->consumePrimitiveData(num_elements), num_elements*4);

	//Now mult stuff together into the scratchspace
	for(unsigned int ii=0; ii < block_info.inputs.size(); ii++){
		float *current_input_buffer = (float*)block_info.inputs[ii]->consumePrimitiveData(num_elements);
		for(int jj=0; jj < num_elements; jj++){
			multed_data_ptr[jj] *= current_input_buffer[jj];
		}
	}
}

void mult::mult_double(int num_elements){
	//Check to see if we have enough space
	if(num_elements*8 > multed_data_size){
		multed_data_size = num_elements*2*8;
		free(multed_data);
		multed_data = malloc(multed_data_size*sizeof(double));
	}
	
	double *multed_data_ptr = (double*)multed_data;
	
	//Start out with the contents of the first input buffer
	memcpy(multed_data_ptr, block_info.inputs[0]->consumePrimitiveData(num_elements), num_elements*8);

	//Now mult stuff together into the scratchspace
	for(unsigned int ii=0; ii < block_info.inputs.size(); ii++){
		double *current_input_buffer = (double*)block_info.inputs[ii]->consumePrimitiveData(num_elements);
		for(int jj=0; jj < num_elements; jj++){
			multed_data_ptr[jj] *= current_input_buffer[jj];
		}
	}
}
