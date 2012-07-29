#include <div.h>
#include <string.h>
#include <cstdlib>

div::div(flowBlockDescription in_desc) : templateArithmetic(in_desc){
}

div::~div(){
}

void div::process(){
	general_arithmetic_process();
}

