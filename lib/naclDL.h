#include <string>

//Preprocessor macro for defining a class which can be dynamically loaded later!
#define DSPFLOW_BLOCK(bn) \
extern "C" flowBlock* create_##bn(){ \
	return new ##bn(); \
} \
extern "C" void delete_##bn(flowBlock *object){ \
	delete object; \
} \
class ##bn{

class naclDL{
public:
	naclDL();
	flowBlock *newDLBlock(std::string library, std::string block_name);
private:
	void *getLibrary(std::string library);
	std::map<std::string,void*> open_libraries;
};
