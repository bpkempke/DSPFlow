#include<socket_server.h>

socket_server::socket_server(){
	int in_socket_port = atoi(block_info.key_value["socket_port"].c_str());
	std::string in_socket_type = block_info.key_value["socket_type"];

	//Figure out what socket type this is actually referring to
	if(in_socket_type.compare("WS") == 0)
		socket_type = SOCKET_WS;
	else if(in_socket_type.compare("TCP") == 0)
		socket_type = SOCKET_TCP;
	else //We don't really want to have to use UDP because it's going to screw with stuff
		socket_type = SOCKET_NULL;

	//Now instantiate the socket interface
	sock = new socketInterface(socket_type);

	//Create a new socket to listen on
	int socket_fd = newSocket(in_socket_port, sock, true);

	sock_trans = new socket_server_intermediary(sock);

	is_running = true;
}

socket_server::~socket_server(){
	is_running = false;

	//Stop all running threads
	close(sock->getSockFP());
	DSPFLOW_STOP_BACKGROUND_THREAD(socket_server);
}

void socket_server::process(){
	//This should send data out to all connected clients
	//TODO: Just copied a lot of this out of file.cc... should probably go somewhere common to the two
	//Just run through all the inputs sequentially and write them all out to file in that order...
	for(int ii=0; ii < block_info.inputs.size(); ii++){
		primType cur_input_primtype = block_info.inputs->getPrimitiveType();
		int num_elements = block_info.inputs->getPrimitiveUsage();
		if(cur_input_primtype == PRIM_VOID){
			//TODO: This is where we would query for the object's byte representation
		} else if(cur_input_primtype == PRIM_INT8){
			char *data = block_info.inputs->consumePrimitiveData(num_elements);
			sock_trans->dataFromUpstream(data, num_elements, NULL);
		} else if(cur_input_primtype == PRIM_INT32){
			int32_t *data = block_info.inputs->consumePrimitiveData(num_elements);
			sock_trans->dataFromUpstream(data, num_elements*sizeof(int32_t, NULL);
		} else if(cur_input_primtype == PRIM_FLOAT){
			float *data = block_info.inputs->consumePrimitiveData(num_elements);
			sock_trans->dataFromUpstream(data, num_elements*sizeof(float), NULL);
		} else if(cur_input_primtype == PRIM_DOUBLE){
			double *data = block_info.inputs->consumePrimitiveData(num_elements);
			sock_trans->dataFromUpstream(data, num_elements*sizeof(double), NULL);
		}
	}

}

void socket_server::got_socket_data(char *buffer, int num_bytes){
	//Put it on the top output flowPipe (ignore the rest if there are any...)
	block_info.outputs[0]->insertPrimitiveData(buffer, num_bytes);
}

flowBlock *socket_server::replicate_block(){
	//TODO: This is sort of a conundrum... what does one do with a socket_server if it's asked to replicate itself?
	// Best option that I can think of would be to increment the address and make a new one...
	// Or you could implement something at the application layer, but that gets messy
}

void socket_server::background_thread(){
	//This thread just waits for incoming connections and then pushes them on to the socketInterface instance!
	// (mostly copied from the SDRPortal project (<sdrportal_repo>/src/uhd_daemon.cc)
	struct sockaddr_in data_cli;
	int datasock_fd, socket_fd;
	socklen_t data_cli_len;

	socket_fd = sock->getSockFP();

	while(1){
		data_cli_len = sizeof(data_cli);
		datasock_fd = accept(socket_fd, (struct sockaddr *) &data_cli, &data_cli_len);

		//likely this is because we got a shutdown request, therefore let's break out of here
		if(datasock_fd < 0)
			break;
		
		//Launch all the threads necessary to keep track of this new socket connection
		new socketThread(datasock_fd, sock, &uplink_lock, false);
	}

}

//TODO: This is particularly inefficient, but don't really know how to 
socketServerIntermediary::socketServerIntermediary(socketInterface *in_sock, flowBlock *in_upstream_block){
	in_sock->registerUpstreamInterface(this);
	upstream_block = in_upstream_block;
}

void socketServerIntermediary::fdBytesReceived(char *buffer, int num_bytes, fdInterface *from_interface){
	//This function is called from the socket interface
	upstream_block->got_socket_data(buffer, num_bytes);
}

void socketServerIntermediary::dataFromUpstream(char *data, int num_bytes, fdInterface *from_interface){
	//This function is called from the socket_server block
	downstream_interface->dataFromUpstream(data, num_bytes, this);
}