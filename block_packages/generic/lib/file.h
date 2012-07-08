#include <stdio.h>

#define FILE_READ_SIZE 8092

enum fileType {
	FILE_READ,
	FILE_WRITE
};

DSPFLOW_BLOCK(file){
public:
	file();
	~file();
	void process();
private:
	void background_thread();
	fileType file_type;
	FILE *file_ptr;
	bool running;
	void *temp_read_data;
};
