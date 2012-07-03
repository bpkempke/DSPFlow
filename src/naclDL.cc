naclDL::naclDL(){

}

flowBlock *naclDL::newDLBlock(std::string library, std::string block_name){
	void *library_handle = getLibrary(library);
	flowBlock* (*create)();
	std::string create_fcname = "create_";
	create_fcname += block_name;
	create = (flowBlock* (*)())dlsym(library_handle, create_fcname.c_str());
	flowBlock *new_block = (flowBlock*)create();
}

//Destroy stuff
//	void (*destroy)(flowBlock*);
//	std::string destroy_fcname = "destroy_"; //TODO: Not sure if the destroy function is really needed, should just be able to call delete?
//	destroy_fcname += block_name;
//	destroy = (void (*)(flowBlock*))dlsym(library_handle, destroy_fcname.c_str());

void *naclDL::getLibrary(std::string library){
	//Try to see if the library is open already...
	std::map<std::string,void*>::iterator find_library = open_libraries.find(library);
	if(find_library == open_libraries.end()){
		//Library not found! Load it in...
		std::string filename = library;
		filename += ".so";
		void *handle = dlopen(filename.c_str(), RTLD_NOW);
		return handle;
	} else {
		//Library found, just give back the pointer
		return find_library->second;
	}

}
