#include <flowBlock.h>

class flowGraph{
public:
	flowGraph(std::string graph_desc_xml);
	flowBlock *addBlock(flowBlockDescription in_desc);
	void addPipe(std::string start_id, bool is_primitive);
	void removeBlock(std::string id);
	void sendMessage(std::string id, std::string message);
	void pipeHasData(flowPipe *in_pipe);
	void run();
	void stop();
private:
	int getBlockIdx(std::string id);
	void thread_run();
	std::vector<flowBlock*> flowgraph_blocks;
	std::vector<flowPipe*> flowgraph_pipes;
	std::map<std::string,int> flowgraph_indices;
	
	//Queues to keep track of which pipes and blocks have data or need processing
	std::vector<flowPipe*> ready_pipes;
	std::vector<flowBlock*> ready_blocks;
	
	//Dynamic linking library
	naclDL *block_library;

	//Variables used to manipulate flowGraph thread state
	pthread_t fg_thread;
	bool running;
};
