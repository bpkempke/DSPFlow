#ifndef NACLDL_H
#define NACLDL_H

#include <string>
#include <map>
#include <flowBlock.h>

class naclDL{
public:
	naclDL();
	flowBlock *newDLBlock(std::string library, std::string block_name, flowBlockDescription in_desc);
private:
	void *getLibrary(std::string library);
	std::map<std::string,void*> open_libraries;
};

#endif
