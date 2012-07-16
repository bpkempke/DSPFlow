#include <flowBlock.h>
#include <socketInterface.h>

class socket_server;
class socketServerIntermediary : public fdInterface{
public:
	socketServerIntermediary(socketInterface *sock_in, socket_server *in_upstream_block);
	void fdBytesReceived(char *buffer, int num_bytes, fdInterface *from_interface);
	void dataFromUpstream(char *data, int num_bytes, fdInterface *from_interface);
private:
	socketInterface *downstream_interface;
	socket_server *upstream_block;
};

class socket_server:public flowBlock{
public:
	socket_server(flowBlockDescription in_desc);
	~socket_server();
	void process();
	void got_socket_data(char *buffer, int num_bytes);
	flowBlock *replicate_block();
	void background_thread();
private:
	bool is_running;

	//socket-specific information
	std::string server_address;
	int server_port;

	//Actual socket interface class
	socketInterface *sock;
	socketIntType socket_type;
	pthread_t conn_listener_thread;
	socketServerIntermediary *sock_trans;
};

DSPFLOW_BLOCK(socket_server);
