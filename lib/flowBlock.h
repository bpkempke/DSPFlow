#ifndef FLOWBLOCK_H
#define FLOWBLOCK_H

#include <string>
#include <vector>
#include <map>
#include <flowPipe.h>

//TODO: Figure out where/how flowGraph::pipeHasData() is called
struct flowBlockDescription{
	std::string id;
	std::string function;
	std::vector<flowPipe*> inputs;
	std::vector<flowPipe*> outputs;
	std::map<std::string,std::string> key_value;
	bool thread_needed;
};

//Virtual class, some of these members need to be overloaded by the actual implementation of the blocks
class flowBlock{
public:
	flowBlock(flowBlockDescription in_description);
	virtual ~flowBlock() =0;
	virtual void process() =0;
	virtual void process_message(std::string in_message){};//TODO: we may want to make messages a class or struct...
	void addOutputPipe(flowPipe* out_pipe);
	void addInputPipe(flowPipe* in_pipe);
	std::string getId();

	virtual void *background_thread(){return NULL;}; //TODO: Would be nice if this wasn't exposed...
	pthread_t back_thread; //TODO: Would be nice if this wasn't exposed...
protected:
	primType getInputPrimitiveType();
	int getMinInputPipeUsage();
	flowBlockDescription block_info;
private:
	void producePrimitiveData(flowPipe *in_pipe, void *data, int num_bytes);
	void producePipeObject(flowPipe *in_pipe, flowPipeObject *in_object);
	void produceKeyValue(flowPipe *in_pipe, std::string in_key, flowPipeObject *in_value);
};
inline flowBlock::~flowBlock(){}

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


#endif
