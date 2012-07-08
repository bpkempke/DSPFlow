#include <flowBlock.h>

DSPFLOW_BLOCK(div){
public:
	div(flowBlockDescription in_desc);
	~div();
	void process();
private:
	primType input_prim_type;
	void (div::*div_fptr)(int);

	//Separate member functions for diving primitives together
	void div_int8(int);
	void div_int32(int);
	void div_float(int);
	void div_double(int);

	//Scratch space for diving stuff together in
	void *dived_data;
	int dived_data_size;
}
