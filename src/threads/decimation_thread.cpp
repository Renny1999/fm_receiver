#include <string>
#include "decimation_thread.h"

using namespace std;

void* decimation_thread(void* args){
	// get parameters
	decimation_args* params = (decimation_args*) args;

	BlockingQueue<complex<float>>* in = params->in;
	BlockingQueue<complex<float>>* out = params->out;
	int dec_rate = params->dec_rate;
	int chunk_size = params->chunk_size;

	complex<float>* data;

	int counter = 0;
	int index = 0;
	complex<float>* decimated = new complex<float>[chunk_size];
	while(true)	{
		data = in->pop()->data;
		for(int i = 0; i < chunk_size; i++){
			// counter is 0 if its the sample to be taken
			if(counter == 0){
				decimated[index] = data[i];
				index++;
				index%=chunk_size;

				if(index == 0){
					out->push(decimated);
					decimated = new complex<float>[chunk_size];
				}
			}
			counter++;
			counter%=dec_rate;
		}

	}
}