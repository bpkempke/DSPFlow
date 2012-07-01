#include <flowBlock.h>

class flowGraph{
public:
	flowGraph(std::string graph_desc_xml);
	void addBlock(flowBlockDescription in_desc);
	void addPipe(std::string start_id, bool is_primitive);
	void removeBlock(std::string id);
	void sendMessage(std::string id, std::string message);
	void run();
	void pause();
private:
	int getBlockIdx(std::string id);
	void thread_run();
	std::vector<flowBlock*> flowgraph_blocks;
	std::vector<flowPipe*> flowgraph_pipes;
	std::map<std::string,int> flowgraph_indices;
};
