#include <string>
#include <vector>
#include <flowPipe.h>

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
	flowBlcokDescription block_info;
};
