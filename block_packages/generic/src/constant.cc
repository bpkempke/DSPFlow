#include <constant.h>
#include <cstdlib>
#include <pthread.h>

constant::constant(flowBlockDescription in_desc) : flowBlock(in_desc){
	
	primType output_primtype = block_info.outputs[0]->getPrimitiveType();
	double constant_value = atof(block_info.key_value["value"].c_str());
	if(output_primtype == PRIM_VOID)
		throw 0; //Void constants are undefined....
	else if(output_primtype == PRIM_INT8){
		constant_storage = new int8_t[1];
		*constant_storage = (int8_t)constant_value;
		num_constant_bytes = sizeof(int8_t);
	} else if(output_primtype == PRIM_INT32){
		constant_storage = new int32_t[1];
		*constant_storage = (int32_t)constant_value;
		num_constant_bytes = sizeof(int32_t);
	} else if(output_primtype == PRIM_FLOAT){
		constant_storage = new float[1];
		*constant_storage = (float)constant_value;
		num_constant_bytes = sizeof(float);
	} else if(output_primtype == PRIM_DOUBLE){
		constant_storage = new double[1];
		*constant_storage = (double)constant_value;
		num_constant_bytes = sizeof(double);
	}

	DSPFLOW_START_BACKGROUND_THREAD(constant);
}

constant::~constant(){
	running = false;
	DSPFLOW_STOP_BACKGROUND_THREAD(constant);
	delete constant_storage;
}

void constant::process(){

}

void *constant::background_thread(){
	running = true;
	while(running){
		//Read a random amount of data
		int n = fread(temp_read_data, 1, FILE_READ_SIZE, file_ptr);
		if(n != FILE_READ_SIZE) running = false;

		//Put it on the top output flowPipe (ignore the rest if there are any...)
		block_info.outputs[0]->insertPrimitiveData(temp_read_data, n);
#ifdef __APPLE__
		pthread_yield_np(); //TODO: is this good enough for Mac???
#else
		pthread_yield();
#endif
		//TODO: May need to put a check in here to make sure that the output doesn't get backed up...
	}
	return NULL;
}
