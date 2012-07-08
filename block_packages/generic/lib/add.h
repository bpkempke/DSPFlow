#include <flowBlock.h>

DSPFLOW_BLOCK(add){
public:
	add(flowBlockDescription in_desc);
	~add();
	void process();
private:
	primType input_prim_type;
	void (add::*add_fptr)(int);

	//Separate member functions for adding primitives together
	void add_int8(int);
	void add_int32(int);
	void add_float(int);
	void add_double(int);

	//Scratch space for adding stuff together in
	void *added_data;
	int added_data_size;
};
