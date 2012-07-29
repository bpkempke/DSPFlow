#ifndef TEMPLATEARITHMETIC_H
#define TEMPLATEARITHMETIC_H
#include <flowBlock.h>
#include <stdio.h>
#include <cstdlib>
#include <string.h>

//TODO: Is there any way to do this without it using the preprocessor??!?!
#define GENERAL_ARITHMETIC(type_in){\
	/*First figure out how much data is ready to add together*/	\
	unsigned int num_elements = getMinInputPipeUsage();		\
									\
	/*Check to see if we have enough space*/ 			\
	if(num_elements*sizeof(type_in) > data_size){			\
		data_size = num_elements*2*sizeof(type_in);		\
		free(data);						\
		data = malloc(data_size*sizeof(type_in));		\
	}								\
									\
	type_in *data_ptr = (type_in*)data;				\
									\
	/*Start out with the contents of the first input buffer*/	\
	memcpy(data_ptr, block_info.inputs[0]->consumePrimitiveData(num_elements), num_elements*sizeof(type_in));\
									\
	/*Now add stuff together into the scratchspace*/		\
	for(unsigned int ii=1; ii < block_info.inputs.size(); ii++){\
		type_in *current_input_buffer = (type_in*)block_info.inputs[ii]->consumePrimitiveData(num_elements);\
		for(unsigned int jj=0; jj < num_elements; jj++){			\
			data_ptr[jj] = ArithmeticOp<type_in>()(data_ptr[jj],current_input_buffer[jj]);\
		}							\
	}								\
	block_info.outputs[0]->insertPrimitiveData(data_ptr, num_elements*sizeof(type_in));\
}

template <template<class T> class ArithmeticOp>
class templateArithmetic : public flowBlock{
public:
	templateArithmetic(flowBlockDescription in_desc);
	~templateArithmetic();
	void general_arithmetic_process();
private:
	primType input_prim_type;
	void (templateArithmetic::*arith_fptr)(void);

	void arith_int8() GENERAL_ARITHMETIC(int8_t);
	void arith_int32() GENERAL_ARITHMETIC(int32_t);
	void arith_float() GENERAL_ARITHMETIC(float);
	void arith_double() GENERAL_ARITHMETIC(double);
	
	//Scratch space for adding stuff together in
	void *data;
	unsigned int data_size;

};

template <template<class T> class ArithmeticOp>
templateArithmetic<ArithmeticOp>::templateArithmetic(flowBlockDescription in_desc) : flowBlock(in_desc){
	input_prim_type = getInputPrimitiveType();
	if(input_prim_type == PRIM_VOID) //TODO: THROW SOMETHING SENSIBLE!
		throw 0;

	//Test this shiznatch out
	//int n = ArithmeticOp<int>()(1,2);

	//Instantiate the correct type of processing function
	if(input_prim_type == PRIM_INT8)
		arith_fptr = &templateArithmetic::arith_int8;
	else if(input_prim_type == PRIM_INT32)
		arith_fptr = &templateArithmetic::arith_int32;
	else if(input_prim_type == PRIM_FLOAT)
		arith_fptr = &templateArithmetic::arith_float;
	else if(input_prim_type == PRIM_DOUBLE)
		arith_fptr = &templateArithmetic::arith_double;

	//Start out with a modestly-sized array for using for temp storage
	data_size = 0;
}

template <template<class T> class ArithmeticOp>
templateArithmetic<ArithmeticOp>::~templateArithmetic(){
	if(data_size)
		free(data);
}

template <template<class T> class ArithmeticOp>
void templateArithmetic<ArithmeticOp>::general_arithmetic_process(){
	(*this.*arith_fptr)();
}
#endif
