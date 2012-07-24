#include <socket_client.h>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg){
	perror(msg);
	exit(1);
}

socket_client::socket_client(flowBlockDescription in_desc): flowBlock(in_desc){

	//Establish the connection to the desired server
	int in_socket_port = atoi(block_info.key_value["socket_port"].c_str());
	std::string in_socket_addr = block_info.key_value["socket_addr"];
	std::string in_socket_type = block_info.key_value["socket_type"];//(ignored)

	//TODO: For now, we're just going to assume that this is a TCP socket
	//TODO: Put in some useful error handling in here...
	//Also, this code is mainly borrowed from http://www.linuxhowtos.org/C_C++/socket.htm
	struct sockaddr_in serv_addr;
	struct hostent *server;
	
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0) 
		error("ERROR opening socket");
	server = gethostbyname(in_socket_addr.c_str());
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
	}
	memset((char *) &serv_addr, 1, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	memcpy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(in_socket_port);
	if (connect(socket_fd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");

	//Set up the background thread which will wait for incoming data from the server
	DSPFLOW_START_BACKGROUND_THREAD(socket_client);
}

socket_client::~socket_client(){
	//Close the socket that we're communicating on...
	close(socket_fd);
	running = false;
	DSPFLOW_STOP_BACKGROUND_THREAD(socket_client);
}

void socket_client::process(){
	//TODO: Again I'm just copy-pasting this from file.cc...
	//Just run through all the inputs sequentially and write them all out to file in that order...
	for(unsigned int ii=0; ii < block_info.inputs.size(); ii++){
		primType cur_input_primtype = block_info.inputs[ii]->getPrimitiveType();
		int num_elements = block_info.inputs[ii]->getPrimitiveUsage();
		int n;
		if(cur_input_primtype == PRIM_VOID){
			//TODO: This is where we would query for the object's byte representation
		} else if(cur_input_primtype == PRIM_INT8){
			char *data = (char*)block_info.inputs[ii]->consumePrimitiveData(num_elements);
			n = write(socket_fd,data,num_elements);
		} else if(cur_input_primtype == PRIM_INT32){
			int32_t *data = (int32_t*)block_info.inputs[ii]->consumePrimitiveData(num_elements);
			n = write(socket_fd,data,num_elements*sizeof(int32_t));
		} else if(cur_input_primtype == PRIM_FLOAT){
			float *data = (float*)block_info.inputs[ii]->consumePrimitiveData(num_elements);
			n = write(socket_fd,data,num_elements*sizeof(float));
		} else if(cur_input_primtype == PRIM_DOUBLE){
			double *data = (double*)block_info.inputs[ii]->consumePrimitiveData(num_elements);
			n = write(socket_fd,data,num_elements*sizeof(double));
		}
		if(n < 0)
			error("ERROR writing to socket");
	}
}

flowBlock *socket_client::replicate_block(){
	//TODO: Not quite sure how to do this at the moment...
	return new socket_client(block_info);
}

void *socket_client::background_thread(){
	running = true;
	while(running){
		//Read a random amount of data
		int n = read(socket_fd, temp_read_data, sizeof(temp_read_data));
		if (n < 0) 
			error("ERROR reading from socket");

		//Put it on the top output flowPipe (ignore the rest if there are any...)
		block_info.outputs[0]->insertPrimitiveData(temp_read_data, n);
	}
	return NULL;
}
