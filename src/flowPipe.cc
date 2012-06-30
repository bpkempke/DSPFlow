//For now, the flowPipe class will be pretty dumb.  
// It's fast unless you don't read everything each time you call consumeBytes... so DO THAT!

//flowPipe constructor
flowPipe::flowPipe(int initial_size){

	//Initialize dynamic data structures
	// Use native system size so that we don't end up with a bunch of unaligned accesses
	//Multiply the amount of memory by two, why will become apparent later (it's faster that way...)
	pipe_data = new char[initial_size];

	//Other variable initializations
	current_usage = 0;
	max_capacity = intial_size;
}

//Push data into the pipe!
int flowPipe::pushData(void *in_data, int num_bytes){
	//Check to see if there's too much data in here
	if(current_usage + num_bytes > max_capacity){
		delete pipe_data;
		max_capacity *= 2;
		pipe_data = new char[max_capacity];
	}

	//Copy in the data
	memcpy(pipe_data+current_usage,in_data,num_bytes);
	current_usage += num_bytes;
}

//Get a pointer to the tail of the queue
void *flowPipe::consumeBytes(int num_bytes){
	void *return_pointer = pipe_data;
	
	current_usage -= num_bytes;
	if(current_usage < 0) current_usage = 0; 

	return return_pointer;
}

//Get the number of used bytes in the pipe
int flowPipe::getUsage(){
	return current_usage;
}
