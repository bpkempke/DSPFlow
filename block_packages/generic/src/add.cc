#include <add.h>
#include <cstdlib>
#include <stdio.h>

add::add(flowBlockDescription in_desc) : templateArithmetic(in_desc){
}

add::~add(){
}

void add::process(){
	general_arithmetic_process();
}

