#include <templateArithmetic.h>
#include <flowBlock.h>

class mult:public templateArithmetic<std::multiplies>{
public:
	mult(flowBlockDescription in_desc);
	~mult();
	void process();
private:

};

DSPFLOW_BLOCK(mult);
