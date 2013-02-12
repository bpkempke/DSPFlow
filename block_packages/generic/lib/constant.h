#include <stdio.h>
#include <flowBlock.h>

class constant:public flowBlock{
public:
	constant(flowBlockDescription in_desc);
	~constant();
	void process();
	void *background_thread();
private:
	bool running;
	void *constant_storage;
	int num_constant_bytes;
};

DSPFLOW_BLOCK(constant);
