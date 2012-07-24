#include <naclDL.h>
#include <dlfcn.h>
#include <stdio.h>

naclDL::naclDL(){

}

flowBlock *naclDL::newDLBlock(std::string library, std::string block_name, flowBlockDescription in_desc){
	void *library_handle = getLibrary(library);
	flowBlock* (*create)(flowBlockDescription);
	std::string create_fcname = "create_";
	create_fcname += block_name;
	create = (flowBlock* (*)(flowBlockDescription))dlsym(library_handle, create_fcname.c_str());
	flowBlock *new_block = (flowBlock*)create(in_desc);
	return new_block;
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
		std::string filename = "../block_packages/generic/src/libDSPFlow_" + library + ".so";
		//TODO: This is a bit of a kludge for now.... figure out how to make this better....
		//TODO: Put in some more error checking here!!!
		void *handle = dlopen(filename.c_str(), RTLD_NOW);
		open_libraries[library] = handle;
		return handle;
	} else {
		//Library found, just give back the pointer
		return find_library->second;
	}

}
