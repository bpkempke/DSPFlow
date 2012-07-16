#include <string>

DSPFLOW_BLOCK(socket_client){
public:
	socket_client();
	~socket_client();
	void process();
	flowBlock *replicate_block();
private:
	void background_thread();
	int socket_fd;
	char temp_read_data[1000];
	bool running;
};
