#include <tinyxml2.h>
#include <pthread.h>

/*************************************
  * flowGraph
  ************************************/

//Proxy function to run a thread inside of a flowGraph member function
static void *flowGraphThreadProxy(void *in_flowgraph){
	static_cast<flowGraph*>(in_flowgraph)->thread_run();
}

flowGraph::flowGraph(std::string graph_desc_xml){
	//First parse the flow graph
	XMLDocument xml_parser;
	xml_parser.Parse(graph_desc_xml.c_str());

	//This text must already have gotten rid of the <flowgraph> tags, so we're left with parsing individual blocks
	XMLElement *block_element;
	for(block_element = xml_parser.FirstChildElement("block"); block_element != NULL; block_element = block_element->NextSiblingElement("block")){
		flowBlockDescription newblock_description;

		XMLElement *id_element = block_element->FirstChildElement("id");
		newblock_description.id = id_element->ToText();
		XMLElement *function_element = block_element->FirstChildElement("function");
		newblock_description.function = function_element->ToText();
		XMLElement *input_element;
		for(input_element = block_element->FirstChildElement("input"); input_element != NULL; input_element = input_element->NextChildElement("input")){
			//TODO: How do we know if we want this to use primitives
			flowPipe *new_pipe = new flowPipe(false);
			//TODO: need to connect this pipe up!
		}
		XMLElement *output_element;
		for(output_element = block_element->FirstChildElement("output"); output_element != NULL; output_element = output_element->NextChildElement("output")){
			//TODO: figure out what to do here!
		}

		//Create a new flowBlock element
		addBlock(newblock_description);
	}

	//Create the main thread to run this flowgraph
	pthread_t new_flowgraph_thread;
	pthread_create(&new_flowgraph_thread, NULL, flowGraphThreadProxy ,this);
	pthread_detach(new_flowgraph_thread);
}

void flowGraph::addBlock(flowBlockDescription in_desc){
	//create the new block and add it onto the list...
	flowBlock *new_block = new flowBlock(in_desc);
	flowgraph_indices[in_desc.id] = flowgraph_blocks.length();
	flowgraph_blocks.push_back(new_block);
}

void flowGraph::addPipe(flowBlock *source_block, bool is_primitive){
	//Create the new pipe and add it onto the list...
	flowPipe *new_pipe = new flowPipe(is_primitive);
	flowgraph_pipes.push_back(new_pipe);

	//Make sure the source block has a reference to the new pipe
	source_block->addOutputPipe(new_pipe);
}

void flowGraph::connectPipe(flowBlock *dest_block, flowPipe *in_pipe, std::string source_id){
	//Get the idx of the requested block
	int source_idx = getBlockIdx(source_id);

	//Now associate the existing pipe with its destination
	flowgraph_blocks[source_idx]->addOutputPipe(in_pipe);

	//TODO: This is neither complete nor completely correct
	
}

int flowGraph::getBlockIdx(std::string id){
	//Figure out which block is associated with the stated dest_id
	return flowgraph_indices[id];
}	

void flowGraph::removeBlock(std::string id){
	//Find and delete the block
	int block_index = flowgraph_indices[id];
	delete flowgraph_blocks[block_index];
	flowgraph_blocks.erase(flowgraph_blocks.begin()+block_index,flowgraph_blocks.begin+block_index+1);
	flowgraph_indices.erase(id);

	//Go through and decrement the index of every block in the map which is higher than the one that's being taken out
	std::map<std::string,int>::iterator fg_it;
	for(fg_it = flowgraph_indices.begin(); fg_it != flowgraph_indices.end(); fg_it++){
		if((*fg_it).second >= block_index)
			(*fg_it).second--;
	}
}

void flowGraph::sendMessage(std::string id, std::string message){
	//Find the block
	int block_index = flowgraph_indices[id];

	//Send the block the message
	flowgraph_blocks[block_index]->process_message(message);
}

void flowGraph::run(){
	//TODO: What should this do?
}

void flowGraph::pause(){
	//TODO: What should this do?
}

void flowGraph::thread_run(){

}

