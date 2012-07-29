#include <templateArithmetic.h>
#include <flowBlock.h>
#include <string.h>

class add:public templateArithmetic<std::plus>{
public:
	add(flowBlockDescription in_desc);
	~add();
	void process();
private:

};

DSPFLOW_BLOCK(add);
