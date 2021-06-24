#include <cstdio>
#include <iostream>
#include <vector>

#include <fftw3.h>
#include <string>
#include "../utils/utils.h"
#include "../utils/FixedSizedDeque.h"
#include "../utils/Deque.h"
#include "LR_diff_extraction_thread.h"

using namespace std;

void* LR_diff_extraction_thread(void* args){
	string name = "L-R EXTRACTION";
	LR_diff_extract_args* params = (LR_diff_extract_args*) args;

	// setup parameters
	BlockingQueue<std::complex<double>>* LR_diff = params->LR_diff;
	BlockingQueue<std::complex<double>>* pilot = params->pilot;
	BlockingQueue<double>* out = params->out;

	int taps = params->taps;
	int Fs = params->sample_rate;
	int chunk_size = params->chunk_size;
	int dec_rate = params->dec_rate;

	// use remez filter for low pass filtering
	vector<complex<double>>* h = read_complex_double_coeffs(params->filter_path_h);
	printf("[%s]	finished setting up filter coeffs\n", name.c_str());

	Deque<complex<double>> x_hist(taps);

	complex<double>* data1;		// L-R chunk
	complex<double>* data2;		// pilot chunk
	double* decimated = new double[chunk_size];

	int counter = 0;
	int index = 0;
	// int c = 0;
	while(true){
        // printf("[%s]   %d\n",name.c_str(), c);
        // c++;

		QueueElement<complex<double>>* popped1 = LR_diff->pop(3000, name);
        if(popped1 == nullptr){
            printf("[%s]    timed out!, exiting thread...\n", name.c_str());
            return nullptr;
        }
		QueueElement<complex<double>>* popped2 = pilot->pop(3000, name);
        if(popped1 == nullptr){
            printf("[%s]    timed out!, exiting thread...\n", name.c_str());
            return nullptr;
        }

		data1 = popped1->data;
		data2 = popped2->data;

		for(int i = 0; i < chunk_size; i++){
			complex<double> d = data1[i]*data2[i];
			x_hist.push_front(d);
			// calculate y[n] only if it is the value needed
			if(counter == 0){
				complex<double> sum(0,0);
				Node<complex<double>>* temp = x_hist.head;

				for(int j = 0; j < taps; j++){
					sum += (*h)[j] * (temp->data);
					temp = temp->next;
				}

				decimated[index] = sum.real();
				index = (index+1) % chunk_size;
				if(index == 0){
					out->push(decimated);
					decimated = new double[chunk_size];
				}
			}
			counter = (counter+1) % dec_rate;
		}
		
		delete popped1;
		delete popped2;
	}// end while
	return nullptr;
}