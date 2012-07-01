
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
