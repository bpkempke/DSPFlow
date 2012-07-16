class socketServerIntermediary : public fdInterface{
public:
	socketServerIntermediary(socketInterface *sock_in, flowBlock *in_upstream_block);
	void fdBytesReceived(char *buffer, int num_bytes, fdInterface *from_interface);
	void dataFromUpstream(char *data, int num_bytes, fdInterface *from_interface);
private:
	socketInterface *downstream_interface;
	flowBlock *upstream_block;
};

DSPFLOW_BLOCK(socket_server){
public:
	socket_server();
	~socket_server();
	void process();
	void got_socket_data(char *buffer, int num_bytes);
	flowBlock *replicate_block();
private:
	void background_thread();
	bool is_running;

	//socket-specific information
	std::string server_address;
	int server_port;

	//Actual socket interface class
	socketInterface *sock;
	socketType socket_type;
	pthread_t conn_listener_thread;
	socketServerIntermediary *sock_trans;
};
