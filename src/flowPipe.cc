
/*************************************
  * flowPipeContainer
  ************************************/
#define DEFAULT_INITIAL_SIZE 65536
//flowPipeContainer constructor
//TODO: probably need a way of not allocating if we don't want a primitive container?
flowPipe::flowPipe(primType primitive_type){
	int initial_size = DEFAULT_INITIAL_SIZE;
	this->primitive_type = primitive_type;

	//Initialize dynamic data structures
	// Use native system size so that we don't end up with a bunch of unaligned accesses
	primitive_data = new char[initial_size];

	//Other variable initializations
	primitive_current_usage = 0;
	primitive_max_capacity = intial_size;
	top_graph_notified = false;
}

//Push data into the pipe!
void flowPipe::insertPrimitiveData(void *in_data, int num_bytes){
	//Check to see if there's too much data in here
	if(primitive_current_usage + num_bytes > primitive_max_capacity){
		delete primitive_data;
		primitive_max_capacity *= 2;
		primitive_data = new char[primitive_max_capacity];
	}

	//Copy in the data
	memcpy(primitive_data+primitive_current_usage,in_data,num_bytes);
	primitive_current_usage += num_bytes;

	//Notify top graph if necessary
	if(!top_graph_notified)
		notifyTopGraph();
}

//Get a pointer to the tail of the queue
void *flowPipe::consumePrimitiveData(int num_bytes){
	void *return_pointer = primitive_data;
	
	primitive_current_usage -= num_bytes;
	if(primitive_current_usage < 0) primitive_current_usage = 0; 

	return return_pointer;
}

//Get the number of used bytes in the pipe
int flowPipe::getPrimitiveUsage(){
	return primitive_current_usage;
}

//Accessor methods for the map
void flowPipe::insertKeyValue(std::string in_key, flowPipeObject *in_value){
	mapped_objects[in_key] = in_value;

	//Notify top graph if necessary
	if(!top_graph_notified)
		notifyTopGraph();
}

flowPipeObject *flowPipe::consumeKeyValue(std::string in_key){
	flowPipeObject *ret = mapped_objects[in_key];
	mapped_objects.erase(in_key);
	return ret;
}

std::string getNextKey(){
	map<std::string, flowPipeObject*>::iterator it;
	it = mapped_objects.begin();
	return (*it).first;
}

//Accessor method for the object pipe
void flowPipe::insertPipeOjbect(flowPipeObject *in_object){
	pipe_objects.push_back(in_object);

	//Notify top graph if necessary
	if(!top_graph_notified)
		notifyTopGraph();
}

flowPipeObject *flowPipe::consumePipeObject(){
	flowPipeObject *ret = pipe_objects[pipe_objects.size()-1];
	pipe_objects.pop_back();
	return ret;
}

int flowPipe::getObjectUsage(){
	return pipe_objects.size();
}

void flowPipe::setInputBlock(flowBlock *in_block){
	this->in_block = in_block;
}

void flowPipe::setOutputBlock(flowBlock *out_block){
	this->out_block = out_block;
}

void flowPipe::setTopGraph(flowGraph *in_graph){
	top_graph = in_graph;
}

void flowPipe::resetTopGraphNotification(){
	top_graph_notified = false;
}

void flowPipe::notifyTopGraph(){
	top_graph->pipeHasData(this);	
}

primType flowPipe::getPrimitiveType(){
	return primitive_type;
}
