#include <limits>

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

	float FLOAT_MIN = std::numeric_limits<float>::lowest();
	char buffer[512] = {0};
	while(true){
		QueueElement<float>* popped = in->pop(3000, name);
		if(popped == nullptr){
			printf("[%s]	timed out! exiting...\n", name.c_str());
			break;
		}

		data = popped->data;

		float max = FLOAT_MIN;
		for(int i = 0; i < chunk_size; i++)	{
			float sample = data[i];
			if(sample > max){
				max = sample;
			}
		}

		for(int i = 0; i < chunk_size; i++){
			float sample = data[i];
			float normalized = sample/max * 32767;
			char* bytes = static_cast<char*>(static_cast<void*>(&normalized));
			
			for(int j = 0; j < 4; j++){
				buffer[buffer_index] = bytes[j];
				buffer_index = (buffer_index+1)%512;
				if(buffer_index == 0){
					// send data out
					send(socket_fd, &buffer, 512, 0);
				}
			}
		}

	}

	return nullptr;
}