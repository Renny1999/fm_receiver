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

using namespace std;

void* networking_thread(void* args){
	string name = "NETWORKING";
	networking_args* params = (networking_args*) args;

	BlockingQueue<float>* in = params->in;
	int chunk_size = params->chunk_size;
	int socket_fd = params->socket_fd;

	int counter = 0;

	float* data;
	int buffer_index;

	FILE* fp;
	FILE* fp2;
	FILE* fp3;
	fp = fopen("output/exp/int_sample.txt", "w");
	// fp2= fopen("output/exp/int_sample_b.txt", "w");
	fp3= fopen("output/exp/int_sample_wb.txt", "wb");

	float FLOAT_MIN = std::numeric_limits<float>::lowest();
	char buffer[512] = {0};
	while(true){
		QueueElement<float>* popped = in->pop(3000, name);
		if(popped == nullptr){
			printf("[%s]	timed out! exiting...\n", name.c_str());
			break;
		}

		data = popped->data;

		for(int i = 0; i < chunk_size; i++){
			float sample = data[i];
			// float normalized = sample/max * 32767;
			sample = sample*32768*0.8;
			if(sample > 32767) sample = 32767;
			if(sample < -32768) sample = -32768;
			int16_t integer = (int16_t) sample;
			fprintf(fp, "%f\n", data[i]);
			// float normalized = sample/max;
			// float normalized = sample;
			// char* bytes = static_cast<char*>(static_cast<void*>(&normalized));
			unsigned char* bytes = reinterpret_cast<unsigned char *>(&integer);
			
			for(int j = 0; j < 2; j++){
				buffer[buffer_index] = bytes[j];
				buffer_index = (buffer_index+1)%512;
				if(buffer_index == 0){
					// send data out
					send(socket_fd, &buffer, 512, 0);
				}
			}
		}

	}
	fclose(fp);
	return nullptr;
}