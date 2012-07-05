#include<string>

//Data flows through pipes using classes derived from this
// flowPipeData class
class flowPipeObject {
public:
	virtual std::vector<char> getObjectByteRepresentation()=0;
	virtual void parseObjectByteRepresentation(std::vector<char> in_vector)=0;
private:

}

//This class implements a generic resizable container which 
// allows an arbitrary amount of data to get backed up before 
// the next block consumes it (well... to a certain extent... 
// there's only a finite amount of memory...)
class flowPipe {
public:
	flowPipe(std::string primitive_type="");

	//TODO: possibly include some locking functionality in here?

	//Primitive data accessor methods
	void insertPrimitiveData(void *in_data, int num_bytes);
	void *consumePrimitiveData(int num_bytes);
	int getPrimitiveUsage();

	//Object data accessor methods
	void insertPipeObject(flowPipeObject *in_object);
	flowPipeObject *getPipeObject();
	int getObjectUsage();

	//Map data accessor methods
	void insertKeyValue(std::string in_key, flowPipeObject *in_value);
	flowPipeObject *consumeKeyValue(std::string in_key);
	std::string getNextKey();

	//Handles to the blocks which own the pipe
	void setInputBlock(flowBlock *in_block);
	void setOutputBlock(flowBlcok *out_block);
	flowBlock *getInputBlock(){return in_block};
	flowBlock *getOutputBlock(){return out_block};

	//Just to make sure the top flowGraph knows when 
	void setTopGraph(flowGraph *in_graph);
	void resetTopGraphNotification();
private:
	void notifyTopGraph();
	flowBlock *in_block;
	flowBlock *out_block;

	//Stuff for upcalls to the top flowGraph object
	flowGraph *top_graph;
	bool top_graph_notified;

	//Things to use for transporting data and/or objects
	char *primitive_data;
	std::string primitive_type;
	int primitive_current_usage;
	int primitive_max_capacity;

	std::vector<flowPipeObject*> pipe_objects;
	std::map<std::string, flowPipeObject*> mapped_objects;
};
