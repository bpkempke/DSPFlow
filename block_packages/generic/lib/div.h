#include <templateArithmetic.h>
#include <flowBlock.h>

class div:public templateArithmetic<std::divides>{
public:
	div(flowBlockDescription in_desc);
	~div();
	void process();
private:

};

DSPFLOW_BLOCK(div);
