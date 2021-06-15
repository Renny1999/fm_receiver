
#include "FM_demod_thread.h"

using namespace std;

void* FM_demod_thread(void* args){
	// read in parameters
	FM_demod_args* params = (FM_demod_args*) args;

	BlockingQueue<complex<float>>* in = params->in;
	BlockingQueue<double>* out = params->out;

	double Fs = params->sample_rate;
	int chunk_size = params->chunK_size;
	
	complex<float> last(0.0, 0.0);

	// while(true){
	for(int i = 0; i < 1000; i++){
		complex<float>* data = in->pop()->data;
		double* fm_demodulated = new double[chunk_size];

		for(int i = 0; i < chunk_size; i++){
			fm_demodulated[i] = arg(data[i]*conj(last));
			last = data[i];
		}

		out->push(fm_demodulated);
	}
}