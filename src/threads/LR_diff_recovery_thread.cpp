#include <iostream>
#include <cstdio>
#include <vector>
#include <fstream>
#include <complex>

#include "LR_diff_recovery_thread.h"
#include "../utils/utils.h"
#include "../utils/Deque.h"

using namespace std;

void* LR_diff_recovery_thread(void* args){
	string name = "LR DIFF RECOV";
	// setup parameters
	LR_diff_recovery_args* params = (LR_diff_recovery_args*) args;
	BlockingQueue<double>* in = params->in;
	BlockingQueue<complex<double>>* out = params->out;

	int Fs = params->sample_rate;
	int chunk_size = params->chunk_size;
	int dec_rate = params->dec_rate;

	vector<complex<double>>* a;
	vector<complex<double>>* b;
	a = read_complex_double_coeffs(params->filter_path_diffeq_a);
	b = read_complex_double_coeffs(params->filter_path_diffeq_b);

	printf("[%s]	initialized filter difference equation\n", name.c_str());
	
	Deque<double> x_hist(b->size());
	Deque<double> y_hist(a->size()-1);

	complex<double> a0 = (*a)[0];
	int counter = 0;
	int index = 0;

	complex<double>* decimated = new complex<double>[chunk_size];
	complex<double>* sig_filtered = new complex<double>[chunk_size];

	int c = 0;
	while(true)	{
		printf("[%s]	%d\n", name.c_str(), c);
		c++;

		QueueElement<double>* popped = in->pop(3000, name);
		if(popped == nullptr){
			printf("[%s]	timed out! exiting thread...\n", name.c_str());
			break;
		}

		double* data = popped->data;

		for(int i = 0; i < chunk_size; i++){
			double d = data[i];
			double x_sum = 0;
			double y_sum = 0;

			x_hist.push_front(d);	

			Node<double>* temp = x_hist.head;

			// apply filter
			for(int coeff_index = 0; coeff_index < b->size(); coeff_index++){
				x_sum+=temp->data * (*b)[coeff_index].real();
				temp = temp->next;
			}

			temp = y_hist.head;
			for(int coeff_index = 1; coeff_index < a->size(); coeff_index++){
				y_sum+=temp->data * (*a)[coeff_index].real();
				temp = temp->next;
			}

			double res = (x_sum-y_sum)/a0.real();
			y_hist.push_front(res);

			sig_filtered[i] = res;

			if(counter == 0){
                decimated[index] = sig_filtered[i];
                index = (index+1) % chunk_size;
                if(index == 0){
                    out->push(decimated);
                    decimated = new complex<double>[chunk_size];
                }
            }
            counter = (counter+1) % dec_rate;
		}
	}// end while

	return nullptr;
}