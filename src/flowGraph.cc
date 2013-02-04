#include <iostream>
#include <sstream>
#include <string>
#include <tinyxml2.h>
#include <pthread.h>
#include <flowGraph.h>

/*************************************
  * flowGraph
  ************************************/

//Proxy function to run a thread inside of a flowGraph member function
static void *flowGraphThreadProxy(void *in_flowgraph){
	return static_cast<flowGraph*>(in_flowgraph)->thread_run();
}

flowGraph::flowGraph(std::string graph_desc_xml){
	//Initialize the block library
	block_library = new naclDL();

	//First parse the flow graph
	tinyxml2::XMLDocument xml_parser;
	xml_parser.Parse(graph_desc_xml.c_str());
	tinyxml2::XMLElement *flowgraph = xml_parser.FirstChildElement("flowgraph");

	//This text must already have gotten rid of the <flowgraph> tags, so we're left with parsing individual blocks
	tinyxml2::XMLElement *block_element;

	//Variables used to keep track of the found pipes, blocks, and their mappings
	int cur_block_index = 0;
	std::map<std::string, int> input_pipe_mappings;
	std::map<std::string, flowPipe *> pipes;
	std::vector<flowBlockDescription> block_descriptions;
	for(block_element = flowgraph->FirstChildElement("block"); block_element != NULL; block_element = block_element->NextSiblingElement("block")){
		flowBlockDescription newblock_description;

		tinyxml2::XMLElement *id_element = block_element->FirstChildElement("id");
		newblock_description.id = id_element->FirstChild()->Value();
		tinyxml2::XMLElement *function_element = block_element->FirstChildElement("function");
		newblock_description.function = function_element->FirstChild()->Value();

		//Get all the key/value pairs
		tinyxml2::XMLElement *input_element;
		for(input_element = block_element->FirstChildElement("arg"); input_element != NULL; input_element = (tinyxml2::XMLElement*)input_element->NextSiblingElement("arg")){
			//Extract the key and value based on '=' delimiter
			std::string cur_arg = input_element->FirstChild()->Value();
			std::string cur_key, cur_value;
			std::stringstream sstream(cur_arg);
			getline(sstream, cur_key, '=');
			getline(sstream, cur_value);

			printf("Key: %s\nValue: %s\n",cur_key.c_str(), cur_value.c_str());
			newblock_description.key_value[cur_key] = cur_value;
		}

		
		for(input_element = block_element->FirstChildElement("input"); input_element != NULL; input_element = (tinyxml2::XMLElement*)input_element->NextSiblingElement("input")){
			//TODO: Does ToText() do what we want it to do here?
			input_pipe_mappings[input_element->FirstChild()->Value()] = cur_block_index;
		}
		tinyxml2::XMLElement *output_element;
		for(output_element = block_element->FirstChildElement("output"); output_element != NULL; output_element = (tinyxml2::XMLElement*)output_element->NextSiblingElement("output")){
			//We need to first determine if this is a complex pipe, or just a plain primitive pipe
			const char *prim_type_str = output_element->Attribute("prim_type");
			primType primitive_type = PRIM_VOID;
			if(!strcmp(prim_type_str,"int8"))
				primitive_type = PRIM_INT8;
			else if(!strcmp(prim_type_str,"int32"))
				primitive_type = PRIM_INT32;
			else if(!strcmp(prim_type_str,"float"))
				primitive_type = PRIM_FLOAT;
			else if(!strcmp(prim_type_str,"double"))
				primitive_type = PRIM_DOUBLE;

			//Create a new pipe, the other end of which will be determined at a later time
			flowPipe *new_pipe = addPipe(primitive_type);
			pipes[output_element->FirstChild()->Value()] = new_pipe;

			//Also add it to the vector of pipes so that we can keep track of it for destruction
			flowgraph_pipes.push_back(new_pipe);

			//We can also link this new pipe into the current block description
			newblock_description.outputs.push_back(new_pipe);
		}

		//Push the current block description onto the stack...
		block_descriptions.push_back(newblock_description);

		//Increment the current block index since we've created a new block!
		cur_block_index++;

	}

	//Now we just have to connect all the inputs
	//TODO Probably would be good if we were able to specify which port inputs/outputs belong to....
	for(std::map<std::string,int>::iterator it=input_pipe_mappings.begin(); it != input_pipe_mappings.end(); it++){
		int which_block = (*it).second;
		flowPipe *which_pipe = pipes[(*it).first];
		block_descriptions[which_block].inputs.push_back(which_pipe);
	}

	//Lastly, instantiate all the blocks...
	for(unsigned int ii=0; ii < block_descriptions.size(); ii++){
		//Create a new flowBlock element
		flowBlock *new_block = addBlock(block_descriptions[ii]);

		//Lastly, go through all the input and output pipe arrays and connect everything together
		for(unsigned int jj=0; jj < block_descriptions[ii].outputs.size(); jj++){
			block_descriptions[ii].outputs[jj]->setInputBlock(new_block);
		}
		for(unsigned int jj=0; jj < block_descriptions[ii].inputs.size(); jj++){
			block_descriptions[ii].inputs[jj]->setOutputBlock(new_block);
		}
		flowgraph_blocks.push_back(new_block);
	}

	//Create the main thread to run this flowgraph
	pthread_t new_flowgraph_thread;
	pthread_create(&new_flowgraph_thread, NULL, flowGraphThreadProxy ,this);
	pthread_detach(new_flowgraph_thread);
}

flowBlock *flowGraph::addBlock(flowBlockDescription in_desc){
	//create the new block and add it onto the list...
	std::stringstream ss(std::stringstream::in | std::stringstream::out);
	ss << in_desc.function;
	std::string newblock_library, newblock_bname;
	std::getline(ss, newblock_library, '/');
	std::getline(ss, newblock_bname);
	printf("Library: %s, Blockname: %s\n", newblock_library.c_str(), newblock_bname.c_str());
	//TODO: error handling if the block function isn't formatted correctly
	flowBlock *new_block = block_library->newDLBlock(newblock_library, newblock_bname, in_desc);
	flowgraph_indices[in_desc.id] = flowgraph_blocks.size();
	flowgraph_blocks.push_back(new_block);

	//Add it to the ready list so that it'll run at some point
	ready_blocks.push_back(new_block);

	return new_block;
}

flowPipe *flowGraph::addPipe(primType primitive_type){
	//Create the new pipe and add it onto the list...
	flowPipe *new_pipe = new flowPipe(primitive_type);
	new_pipe->setTopGraph(this);
	flowgraph_pipes.push_back(new_pipe);

	return new_pipe;
}

void flowGraph::connectPipe(flowBlock *dest_block, flowPipe *in_pipe, std::string source_id){
	//Get the idx of the requested block
	unsigned int source_idx = getBlockIdx(source_id);

	//Now associate the existing pipe with its destination
	flowgraph_blocks[source_idx]->addOutputPipe(in_pipe);

	//TODO: This is neither complete nor completely correct
	
}

unsigned int flowGraph::getBlockIdx(std::string id){
	//Figure out which block is associated with the stated dest_id
	return flowgraph_indices[id];
}	

void flowGraph::removeBlock(std::string id){
	//Find and delete the block
	unsigned int block_index = flowgraph_indices[id];
	delete flowgraph_blocks[block_index];
	flowgraph_blocks.erase(flowgraph_blocks.begin()+block_index,flowgraph_blocks.begin()+block_index+1);
	flowgraph_indices.erase(id);

	//Go through and decrement the index of every block in the map which is higher than the one that's being taken out
	std::map<std::string,unsigned int>::iterator fg_it;
	for(fg_it = flowgraph_indices.begin(); fg_it != flowgraph_indices.end(); fg_it++){
		if((*fg_it).second >= block_index)
			(*fg_it).second--;
	}
}

void flowGraph::sendMessage(std::string id, std::string message){
	//Find the block
	unsigned int block_index = flowgraph_indices[id];

	//Send the block the message
	flowgraph_blocks[block_index]->process_message(message);
}

void flowGraph::pipeHasData(flowPipe *in_pipe){
	//Put the pointer on the list of pipes that have had data inserted into them
	ready_pipes.push_back(in_pipe);
}

void flowGraph::run(){
	//TODO: What should this do?
	pthread_create( &fg_thread, NULL, flowGraphThreadProxy, (void*) this);
	running = true;
}

void flowGraph::stop(){
	running = false;
	pthread_join(fg_thread, NULL);
	//TODO: Does this actually call the destructors of all the flowBlocks??
}

//This method performs all of the actual scheduling of computation, etc.
void *flowGraph::thread_run(){
	while(running){
//		printf("running within the flowGraph thread...\n");
		//Start by running all of the blocks which are ready to go
		while(ready_blocks.size()){
			flowBlock *cur_ready_block = ready_blocks.back();
			ready_blocks.pop_back();
			cur_ready_block->process();
		}

		//Then there may be data in pipes, so call all the blocks which may have been enabled by the pipe's sudden data inflow
		//TODO: Maybe process() shouldn't be called in some cases because it'll just waste time?!
		while(ready_pipes.size()){
			flowPipe *cur_ready_pipe = ready_pipes.back();
			ready_pipes.pop_back();
			flowBlock *enabled_block = cur_ready_pipe->getOutputBlock();
			enabled_block->process();
		}

		//TODO: Maybe we should sleep here sometimes???
	}
	return NULL;
}

