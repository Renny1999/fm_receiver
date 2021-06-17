#include <cmath>
#include "FM_demod_thread.h"

using namespace std;

void* FM_demod_thread(void* args){
	// read in parameters
	FM_demod_args* params = (FM_demod_args*) args;

	BlockingQueue<complex<float>>* in = params->in;
	BlockingQueue<double>* out = params->out;

	double Fs = params->sample_rate;
	int chunk_size = params->chunK_size;
	
	// create de-emphasis filter coefficients:
	double d = Fs*75e-6;	// number of samples to hit the -3dB point
	double x = exp(-1/d);	// decay between each sample
	double b[] = {1-x};		// coefficient for x
	double a[] = {1, -x};	// coefficients for y


	complex<float> last(0.0, 0.0);
	double y_1 = 0.0;		// y_1 means y[n-1], y1 means y[n+1]
	// while(true){
	for(int i = 0; i < 1000; i++){
		complex<float>* data = in->pop()->data;
		double* fm_demodulated = new double[chunk_size];
		double* de_emphasized = new double[chunk_size];

		// apply FM demodulation
		for(int i = 0; i < chunk_size; i++){
			fm_demodulated[i] = arg(data[i]*conj(last));
			last = data[i];
		}

		// apply de_emphasis filter
		for(int i = 0; i < chunk_size; i++){
			// yn = b0xn - a1y_1
			de_emphasized[i] = (b[0] * fm_demodulated[i] - a[1] * y_1)/a[0];
			y_1 = de_emphasized[i];
		}

		out->push(de_emphasized);

		delete[] data;
	}
}