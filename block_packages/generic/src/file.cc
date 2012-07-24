#include <file.h>
#include <cstdlib>
#include <pthread.h>

file::file(flowBlockDescription in_desc) : flowBlock(in_desc){
	file_ptr = NULL;
	
	//Figure out whether this file block is input or output (both is undefined
	if((block_info.inputs.size() != 0) && (block_info.outputs.size() != 0)){
		throw 0; //TODO: Throw something sensible instead...
	} else if(block_info.inputs.size() != 0){
		//This block will write to a file
		file_type = FILE_WRITE;
		file_ptr = fopen(block_info.key_value["file_name"].c_str(), "a");
	} else if(block_info.outputs.size() != 0){
		//This block will read from a file
		file_type = FILE_READ;
		file_ptr = fopen(block_info.key_value["file_name"].c_str(), "r");
		//Allocate a scratchspace for file data
		temp_read_data = malloc(FILE_READ_SIZE);
		//Instantiate the thread which will be reading from the file
		DSPFLOW_START_BACKGROUND_THREAD(file);
	} else {
		//No inputs or outputs?  What do you expect me to DO?!
		throw 0; //
	}
}

file::~file(){
	running = false;
	DSPFLOW_STOP_BACKGROUND_THREAD(file);
	if(file_type == FILE_READ)
		free(temp_read_data);
	if(file_ptr)
		fclose(file_ptr);
}

void file::process(){
	if(file_type == FILE_READ){
		//We should never get here...
	} else {
		//Just run through all the inputs sequentially and write them all out to file in that order...
		for(int ii=0; ii < block_info.inputs.size(); ii++){
			primType cur_input_primtype = block_info.inputs[ii]->getPrimitiveType();
			int num_elements = block_info.inputs[ii]->getPrimitiveUsage();
			if(cur_input_primtype == PRIM_VOID){
				//TODO: This is where we would query for the object's byte representation
			} else if(cur_input_primtype == PRIM_INT8){
				char *data = (char*)block_info.inputs[ii]->consumePrimitiveData(num_elements);
				fwrite(data, 1, num_elements, file_ptr);
			} else if(cur_input_primtype == PRIM_INT32){
				int32_t *data = (int32_t*)block_info.inputs[ii]->consumePrimitiveData(num_elements);
				fwrite(data, sizeof(int32_t), num_elements, file_ptr);
			} else if(cur_input_primtype == PRIM_FLOAT){
				float *data = (float*)block_info.inputs[ii]->consumePrimitiveData(num_elements);
				fwrite(data, sizeof(float), num_elements, file_ptr);
			} else if(cur_input_primtype == PRIM_DOUBLE){
				double *data = (double*)block_info.inputs[ii]->consumePrimitiveData(num_elements);
				fwrite(data, sizeof(double), num_elements, file_ptr);
			}
		}
	}
}

void file::background_thread(){
	running = true;
	while(running){
		//Read a random amount of data
		fread(temp_read_data, 1, FILE_READ_SIZE, file_ptr);

		//Put it on the top output flowPipe (ignore the rest if there are any...)
		block_info.outputs[0]->insertPrimitiveData(temp_read_data, FILE_READ_SIZE);
		pthread_yield();
		//TODO: May need to put a check in here to make sure that the output doesn't get backed up...
	}
}
