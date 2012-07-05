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
	//Initialize the block library
	block_library = new naclDL();

	//First parse the flow graph
	XMLDocument xml_parser;
	xml_parser.Parse(graph_desc_xml.c_str());

	//This text must already have gotten rid of the <flowgraph> tags, so we're left with parsing individual blocks
	XMLElement *block_element;
	std::map<std::string, flowPipe*> pipes;
	for(block_element = xml_parser.FirstChildElement("block"); block_element != NULL; block_element = block_element->NextSiblingElement("block")){
		flowBlockDescription newblock_description;

		XMLElement *id_element = block_element->FirstChildElement("id");
		newblock_description.id = id_element->ToText();
		XMLElement *function_element = block_element->FirstChildElement("function");
		newblock_description.function = function_element->ToText();

		//Create a new flowBlock element
		flowBlock *new_block = addBlock(newblock_description);
		
		XMLElement *input_element;
		for(input_element = block_element->FirstChildElement("input"); input_element != NULL; input_element = input_element->NextChildElement("input")){
			//TODO: Does ToText() do what we want it to do here?
			flowPipe *from_pipe = pipes[input_element->ToText()];
			from_pipe->setOutputBlock(new_block);
		}
		XMLElement *output_element;
		for(output_element = block_element->FirstChildElement("output"); output_element != NULL; output_element = output_element->NextChildElement("output")){
			//We need to first determine if this is a complex pipe, or just a plain primitive pipe
			XMLElement *type_element = output_element->FirstChildElement("primitive_type");

			//Create a new pipe, the other end of which will be determined at a later time
			flowPipe *new_pipe;
			if(type_element != NULL){
				new_pipe = new flowPipe(true, type_element->ToText());
			} else {
				new_pipe = new flowPipe(false);
			}
			new_pipe.setInputBlock(new_block);

			//Add it to the map so that when it's later referenced, we know what pipe we're talking about!
			//TODO: does ToText() do what we want it to do here?
			pipes[output_element->ToText()] = new_pipe;

			//Also add it to the vector of pipes so that we can keep track of it for destruction
			flowgraph_pipes.push_back(new_pipe);

		}

	}

	//Create the main thread to run this flowgraph
	pthread_t new_flowgraph_thread;
	pthread_create(&new_flowgraph_thread, NULL, flowGraphThreadProxy ,this);
	pthread_detach(new_flowgraph_thread);
}

flowBlock *flowGraph::addBlock(flowBlockDescription in_desc){
	//create the new block and add it onto the list...
	//TODO: This needs to use the naclDL object instead of this funniness
	flowBlock *new_block = new flowBlock(in_desc);
	flowgraph_indices[in_desc.id] = flowgraph_blocks.length();
	flowgraph_blocks.push_back(new_block);

	//Add it to the ready list so that it'll run at some point
	ready_blocks.push_back(new_block);

	return new_block;
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

void flowGraph::pipeHasData(flowPipe *in_pipe){
	//Put the pointer on the list of pipes that have had data inserted into them
	ready_pipes.push_back(in_pipe);
}

void flowGraph::run(){
	//TODO: What should this do?
	int ret = pthread_create( &fg_thread, NULL, flowGraphThreadProxy, (void*) this);
	running = true;
}

void flowGraph::stop(){
	running = false;
	pthread_join(fg_thread, NULL);
}

//This method performs all of the actual scheduling of computation, etc.
void flowGraph::thread_run(){
	while(running){
		//Start by running all of the blocks which are ready to go
		while(ready_blocks.size()){
			flowBlock *cur_ready_block = ready_blocks.pop_back();
			cur_ready_block->process();
		}

		//Then there may be data in pipes, so call all the blocks which may have been enabled by the pipe's sudden data inflow
		//TODO: Maybe process() shouldn't be called in some cases because it'll just waste time?!
		while(ready_pipes.size()){
			flowPipe *cur_ready_pipe = ready_pipes.pop_back();
			flowBlock *enabled_block = cur_ready_pipe->getOutputBlock();
			enabled_block->process();
		}

		//TODO: Maybe we should sleep here sometimes???
	}
}

