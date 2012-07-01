#include<string>

//Data flows through pipes using classes derived from this
// flowPipeData class
class flowPipeContainer {
public:
	flowPipeContainer(std::string container_type);
	~flowPipeContainer();
	void insertPrimitiveData(void *in_data, int num_bytes);
	void consumePrimitiveData(int num_bytes);
	int getPrimitiveUsage();
	bool getComplete();
	void setComplete();
	void setMappedData(std::string in_key, void *in_value);
	void *getMappedData(std::string in_key);

private:
	bool complete_flag;

	//Container type and key-value mapping
	std::string container_type;
	map<std::string,void*> data_map;
	
	//Things to use if we are transporting primitive data
	char *primitive_data;
	int primitive_current_usage;
	int primitive_max_capacity;
}

//This class implements a generic resizable container which 
// allows an arbitrary amount of data to get backed up before 
// the next block consumes it (well... to a certain extent... 
// there's only a finite amount of memory...)
class flowPipe {
public:
	flowPipe(bool make_default_container, std::string default_type="");
	flowPipeContainer *getDefaultContainer();
	flowPipeContainer *makeNewContainer(std::string container_type);
	flowPipeContainer *getTopContainer();
	void removeCompleteContainers();
private:
	list<flowPipeContainer> containers;
};
