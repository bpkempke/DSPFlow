#include <stdio.h>
#include <flowBlock.h>

#define FILE_READ_SIZE 8092

enum fileType {
	FILE_READ,
	FILE_WRITE
};

class file:public flowBlock{
public:
	file(flowBlockDescription in_desc);
	~file();
	void process();
	void *background_thread();
private:
	fileType file_type;
	FILE *file_ptr;
	bool running;
	void *temp_read_data;
};

DSPFLOW_BLOCK(file);
