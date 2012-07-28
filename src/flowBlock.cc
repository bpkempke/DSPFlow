#include <flowBlock.h>
#include <stdio.h>

//flowBlock constructor:
flowBlock::flowBlock(flowBlockDescription in_description){
	block_info = in_description;
}

void flowBlock::addOutputPipe(flowPipe *out_pipe){
	block_info.outputs.push_back(out_pipe);
}

void flowBlock::addInputPipe(flowPipe *in_pipe){
	block_info.inputs.push_back(in_pipe);
}

std::string flowBlock::getId(){
	return block_info.id;
}

int flowBlock::getMinInputPipeUsage(){
	//Loop through all the input pipes, looking for the minimum number of primitive datapoints
	int min = block_info.inputs[0]->getPrimitiveUsage();
	for(unsigned int ii=1; ii < block_info.inputs.size(); ii++){
		int candidate = block_info.inputs[ii]->getPrimitiveUsage();
		if(candidate < min) min = candidate;
	}
	return min;
}

primType flowBlock::getInputPrimitiveType(){
	primType ret_type = block_info.inputs[0]->getPrimitiveType();
	for(unsigned int ii = 1; ii < block_info.inputs.size(); ii++){
		primType cur_pipe_type = block_info.inputs[ii]->getPrimitiveType();
		if(cur_pipe_type != ret_type)
			ret_type = PRIM_VOID;
	}
	return ret_type;
}
