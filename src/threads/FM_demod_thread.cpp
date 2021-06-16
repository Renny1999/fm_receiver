#include <cmath>
#include "FM_demod_thread.h"

using namespace std;

void* FM_demod_thread(void* args){
	// read in parameters
	FM_demod_args* params = (FM_demod_args*) args;

	BlockingQueue<complex<float>>* in = params->in;
	BlockingQueue<double>* left_out = params->left_out;
	BlockingQueue<double>* right_out = params->right_out;

	double Fs = params->sample_rate;
	int chunk_size = params->chunK_size;
	double left_offset = 23*1000;	// 23 kHz
	double right_offset = 38*1000;	// 38 kHz
	
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


		// I think I can demodulate the signal to left & right channels 
		// here since it's only 2 extra for loops

		complex<double>* left_channel = new complex<double>[chunk_size];
		complex<double>* right_channel = new complex<double>[chunk_size];

		// e^-j2pif dt
		for(int i = 0; i < chunk_size; i++){
			left_channel[i] = complex<double>(de_emphasized[i], 0)*exp(complex<double>(0, -1*i*2*M_PI*left_offset/Fs));
		}
	
		left_out->push(de_emphasized);
		delete[] data;
	}
}