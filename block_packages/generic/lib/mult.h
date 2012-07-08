#include <flowBlock.h>

DSPFLOW_BLOCK(mult){
public:
	mult(flowBlockDescription in_desc);
	~mult();
	void process();
private:
	primType input_prim_type;
	void (mult::*mult_fptr)(int);

	//Separate member functions for adding primitives together
	void mult_int8(int);
	void mult_int32(int);
	void mult_float(int);
	void mult_double(int);

	//Scratch space for adding stuff together in
	void *multed_data;
	int multed_data_size;
}
