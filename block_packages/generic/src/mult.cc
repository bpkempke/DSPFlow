#include <mult.h>
#include <string.h>
#include <cstdlib>

mult::mult(flowBlockDescription in_desc) : templateArithmetic(in_desc){
}

mult::~mult(){
}

void mult::process(){
	general_arithmetic_process();
}

