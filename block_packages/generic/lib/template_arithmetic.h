#include <flowBlock.h>

template <class T, template<class T> class ArithmeticOp>
class template_arithmetic{
public:
	template_arithmetic(flowBlock *parent_flowblock);
	~template_arithmetic();
	void process();
private:
	flowBlock *parent_block;
	//Scratch space for adding stuff together in
	void *data;
	int data_size;
};

