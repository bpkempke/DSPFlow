#include <string>

//Preprocessor macro for defining a class which can be dynamically loaded later!
#define DSPFLOW_BLOCK(bn) \
extern "C" flowBlock* create_##bn(){ \
	return new ##bn(); \
} \
extern "C" void delete_##bn(flowBlock *object){ \
	delete object; \
} \
void *##bn_thread_proxy(void *ptr){ \
	static_cast<##bn*>(ptr)->background_thread();
} \
class ##bn{

#define DSPFLOW_START_BACKGROUND_THREAD(bn) \
	pthread_create(&back_thread, NULL, ##bn_thread_proxy, (void*)this);

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
