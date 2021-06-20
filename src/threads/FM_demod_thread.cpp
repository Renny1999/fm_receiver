#include <cmath>
#include <string>

#include "FM_demod_thread.h"

using namespace std;

void* FM_demod_thread(void* args){
	string name = "FM DEMOD";

	// read in parameters
	FM_demod_args* params = (FM_demod_args*) args;

	BlockingQueue<complex<float>>* in = params->in;
	BlockingQueue<double>* out1 = params->out1;
	BlockingQueue<double>* out2 = params->out2;
	BlockingQueue<double>* out3 = params->out3;

	double Fs = params->sample_rate;
	int chunk_size = params->chunK_size;
	
	// create de-emphasis filter coefficients:
	double d = Fs*75e-6;	// number of samples to hit the -3dB point
	double x = exp(-1/d);	// decay between each sample
	double b[] = {1-x};		// coefficient for x
	double a[] = {1, -x};	// coefficients for y


	complex<float> last(0.0, 0.0);
	double y_1 = 0.0;		// y_1 means y[n-1], y1 means y[n+1]

	while(true){
		QueueElement<complex<float>>* popped = in->pop(3000, name);
		if(popped == nullptr){
			cout<<"[FM DEMOD]	time out!"<<endl;
			return nullptr;
		}

		complex<float>* data = popped->data;
		double* fm_demodulated1 = new double[chunk_size];
		double* fm_demodulated2 = new double[chunk_size];
		double* de_emphasized = new double[chunk_size];

		// apply FM demodulation
		for(int i = 0; i < chunk_size; i++){
			double res = arg(data[i]*conj(last));
			fm_demodulated1[i] = res;
			// fm_demodulated2[i] = res;
			last = data[i];
		}
		
		/* add a second channel that takes out fm_demodulated */
		// out2->push(fm_demodulated1);
		// out3->push(fm_demodulated2);

		// apply de_emphasis filter
		for(int i = 0; i < chunk_size; i++){
			// yn = b0xn - a1y_1
			de_emphasized[i] = (b[0] * fm_demodulated1[i] - a[1] * y_1)/a[0];
			y_1 = de_emphasized[i];
		}

		out1->push(de_emphasized);
	}

	return nullptr;
}