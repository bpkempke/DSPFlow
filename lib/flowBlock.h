#include <string>
#include <vector>
#include <naclDL.h>
#include <flowPipe.h>

//TODO: Figure out where/how flowGraph::pipeHasData() is called
struct flowBlockDescription{
	std::string id;
	std::string function;
	std::vector<flowPipe*> inputs;
	std::vector<flowPipe*> outputs;
	bool thread_needed;
};

//Virtual class, some of these members need to be overloaded by the actual implementation of the blocks
class flowBlock{
public:
	flowBlock(flowBlockDescription in_description);
	virtual void process() =0;
	virtual void process_message(std::string in_message){};//TODO: we may want to make messages a class or struct...
	void addOutputPipe(flowPipe* out_pipe);
	void addInputPipe(flowPipe* in_pipe);
	std::string getId();
private:
	virtual void background_thread(){};
	void producePrimitiveData(flowPipe *in_pipe, void *data, int num_bytes);
	void producePipeObject(flowPipe *in_pipe, flowPipeObject *in_object);
	void produceKeyValue(flowPipe *in_pipe, std::string in_key, flowPipeObject *in_value);
	int getMinInputPipeUsage();
	primType getInputPrimitiveType();
	flowBlockDescription block_info;
	pthread_t back_thread;
};
