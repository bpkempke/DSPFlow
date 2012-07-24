#include <string>
#include <flowBlock.h>

class socket_client: public flowBlock{
public:
	socket_client(flowBlockDescription in_desc);
	~socket_client();
	void process();
	flowBlock *replicate_block();
	void *background_thread();
private:
	int socket_fd;
	char temp_read_data[1000];
	bool running;
};

DSPFLOW_BLOCK(socket_client);
