#ifndef NACLDL_H
#define NACLDL_H

#include <string>
#include <map>
class flowBlock;

//Preprocessor macro for defining a class which can be dynamically loaded later!
#define DSPFLOW_BLOCK(bn) \
extern "C" flowBlock* create_##bn(flowBlockDescription in_desc){ \
	return new class bn(in_desc); \
} \
extern "C" void delete_##bn(flowBlock *object){ \
	delete object; \
} \
void* bn##_thread_proxy(void *ptr){ \
	return static_cast<class bn*>(ptr)->background_thread(); \
}

#define DSPFLOW_START_BACKGROUND_THREAD(bn) \
	pthread_create(&back_thread, NULL, bn##_thread_proxy, (void*)this);

#define DSPFLOW_STOP_BACKGROUND_THREAD(bn) \
	pthread_join(back_thread, NULL);

class naclDL{
public:
	naclDL();
	flowBlock *newDLBlock(std::string library, std::string block_name);
private:
	void *getLibrary(std::string library);
	std::map<std::string,void*> open_libraries;
};

#endif
