
/*************************************
  * flowPipeContainer
  ************************************/
#define DEFAULT_INITIAL_SIZE 65536
//flowPipeContainer constructor
//TODO: probably need a way of not allocating if we don't want a primitive container?
flowPipeContainer(std::string container_type){
	int initial_size = DEFAULT_INITIAL_SIZE;

	//Initialize dynamic data structures
	// Use native system size so that we don't end up with a bunch of unaligned accesses
	primitive_data = new char[initial_size];

	//Other variable initializations
	primitive_current_usage = 0;
	primitive_max_capacity = intial_size;
	complete_flag = false;
}

//Push data into the pipe!
void flowPipeContainer::insertPrimitiveData(void *in_data, int num_bytes){
	//Check to see if there's too much data in here
	if(primitive_current_usage + num_bytes > primitive_max_capacity){
		delete primitive_data;
		primitive_max_capacity *= 2;
		primitive_data = new char[primitive_max_capacity];
	}

	//Copy in the data
	memcpy(primitive_data+primitive_current_usage,in_data,num_bytes);
	primitive_current_usage += num_bytes;
}

//Get a pointer to the tail of the queue
void *flowPipeContainer::consumePrimitiveData(int num_bytes){
	void *return_pointer = primitive_data;
	
	primitive_current_usage -= num_bytes;
	if(primitive_current_usage < 0) primitive_current_usage = 0; 

	return return_pointer;
}

//Get the number of used bytes in the pipe
int flowPipeContainer::getPrimitiveUsage(){
	return primitive_current_usage;
}

//Set and/or get the complete flag
bool flowPipeContainer::getComplete(){
	return complete_flag;
}

bool flowPipeContainer::setComplete(){
	complete_flag = true;
}

//Accessor methods for the map
void setMappedData(std::string in_key, void *in_value){
	data_map[in_key] = in_value;
}

void *getMappedData(std::string in_key){
	return data_map[in_key];
}

/*************************************
  * flowPipe
  ************************************/
flowPipe::flowPipe(bool make_default_container, std::string default_type){
	if(make_default_container){
		//TODO: is there a way to do this in just one line?!
		flowPipeContainer new_container(default_type);
		containers.push_front(new_container);
	}
}

flowPipeContainer *flowPipe::getDefaultContainer(){
	//For now, just assume that there's only a default container if this method is being called
	//TODO: maybe this isn't a correct assumption?
	return containers.back();
}

flowPipeContainer *flowPipe::makeNewContainer(std::string container_type){
	flowPipeContainer new_container(container_type);
	containers.push_front(new_container);
	return containers.front();
}

flowPipeContainer *flowPipe::getTopContainer(){
	return containers.back();
}

void flowPipe::removeCompleteContainers(){
	list<flowPipeContainer>::iterator cont_it;
	for(cont_it = containers.begin(); cont_it != containers.end(); cont_it++){
		if(cont_it.getComplete()){
			containers.erase(cont_it);
			cont_it--;
		}
	}
}
