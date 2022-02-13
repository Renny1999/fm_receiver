#include <limits>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>
#include <iostream>
#include <cmath>

#include "networking_thread.h"
#include "../utils/utils.h"

using namespace std;

void* networking_thread(void* args){
	string name = "NETWORKING";
	networking_args* params = (networking_args*) args;

	BlockingQueue<double>* LRsum = params->LRsum; /* mono audio */
	BlockingQueue<double>* LRdiff = params->LRdiff;
	int chunk_size = params->chunk_size;
	int socket_fd = params->socket_fd;

	int counter = 0;

	double* data1;
	double* data2;
	int buffer_index = 0;

	float FLOAT_MIN = std::numeric_limits<float>::lowest();
	char buffer[512] = {0};

	// while(!params->exit_loop->load()){
	while(true){

		QueueElement<double>* popped1 = LRsum->pop(10000, name);
		if(popped1 == nullptr){
			printf("[%s]\t\ttimed out! waiting for LRsum exiting...\n", name.c_str());
			break;
		}
		QueueElement<double>* popped2 = LRdiff->pop(10000, name);
		if(popped2 == nullptr){
			printf("[%s]\t\ttimed out waiting for LRdiff exiting...\n", name.c_str());
			break;
		}

		data1 = popped1->data;
		data2 = popped2->data;

		if(params->socket_fd != -1){
			for(int i = 0; i < chunk_size; i++){
				double LRsum_sample = data1[i];
				double LRdiff_sample = data2[i];
				//double LRdiff_sample = data1[i];

				//float left = float(LRsum_sample + LRdiff_sample)*0.8;
				//float right = float(LRsum_sample - LRdiff_sample)*0.8;
				float left = float(LRsum_sample)*0.8;
				float right = float(LRsum_sample)*0.8;

				int left_int = float2int16(left);
				int right_int = float2int16(right);
				unsigned char* left_bytes = reinterpret_cast<unsigned char *>(&left_int);
				unsigned char* right_bytes = reinterpret_cast<unsigned char *>(&right_int);

				for(int j = 0; j < 2; j++){
					buffer[buffer_index] = left_bytes[j];
					buffer_index = (buffer_index+1)%512;
					if(buffer_index == 0){
						// send data out
						send(socket_fd, &buffer, 512, 0);
					}
				}
				for(int j = 0; j < 2; j++){
					buffer[buffer_index] = right_bytes[j];
					buffer_index = (buffer_index+1)%512;
					if(buffer_index == 0){
						// send data out
						send(socket_fd, &buffer, 512, 0);
					}
				}
			}
		} 
		delete popped1;
		delete popped2;
	}// end while
	return nullptr;
}
