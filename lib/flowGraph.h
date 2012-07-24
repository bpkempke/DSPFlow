#ifndef FLOWGRAPH_H
#define FLOWGRAPH_H

#include <flowBlock.h>
#include <flowPipe.h>
#include <naclDL.h>

class flowGraph{
public:
	flowGraph(std::string graph_desc_xml);
	flowBlock *addBlock(flowBlockDescription in_desc);
	void addPipe(flowBlock *source_block, primType primitive_type);
	void connectPipe(flowBlock *dest_bock, flowPipe *in_pipe, std::string source_id);
	void removeBlock(std::string id);
	void sendMessage(std::string id, std::string message);
	void pipeHasData(flowPipe *in_pipe);
	void run();
	void stop();
	void *thread_run(); //TODO: This really probably shouldn't be public, but it has to be in order to get the proxy to work correctly... hmmm....
private:
	unsigned int getBlockIdx(std::string id);
	std::vector<flowBlock*> flowgraph_blocks;
	std::vector<flowPipe*> flowgraph_pipes;
	std::map<std::string,unsigned int> flowgraph_indices;
	
	//Queues to keep track of which pipes and blocks have data or need processing
	std::vector<flowPipe*> ready_pipes;
	std::vector<flowBlock*> ready_blocks;
	
	//Dynamic linking library
	naclDL *block_library;

	//Variables used to manipulate flowGraph thread state
	pthread_t fg_thread;
	bool running;
};

#endif
