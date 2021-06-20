#include <iostream>
#include <cstdio>
#include <vector>
#include <fstream>

#include "pilot_extraction_thread.h"
#include "../utils/BlockingQueue.h"
#include "../utils/Deque.h"
#include "../utils/utils.h"

using namespace std;

void* pilot_extraction_thread_h(void* args){
	string name = "PILOT EXTRACTION";
	// setup parameters 
	pilot_extract_args* params = (pilot_extract_args*) args;
	BlockingQueue<double>* in = params->in;
	BlockingQueue<complex<double>>* out = params->out;

	int Fs = params->sample_rate;
	int chunk_size = params->chunk_size;
	int dec_rate = params->dec_rate;
	int taps = params->taps;

	double* data;

	vector<float>* h = read_float_coeffs(params->filter_path_h);
	Deque<double> x_hist(taps);

	complex<double>* decimated = new complex<double>[chunk_size];
	int counter = 0;
	int index = 0;
	FILE* fp;
	fp = fopen("output/exp/pilot.txt", "w");
	while(true){
		QueueElement<double>* popped = in->pop(3000, name);
		if(popped == nullptr){
			printf("[%s]	timed out!, exiting thread...\n", name.c_str());
			break;
		}

		data = popped->data;

		for(int i = 0; i < chunk_size; i++){
			x_hist.push_front(data[i]);

			if(counter == 0){
				double sum = 0;
				Node<double>* temp = x_hist.head;
				for(int j = 0; j < taps; j++){
					sum += (*h)[j] * (temp->data);
					temp = temp->next;
				}
				decimated[index] = complex<double>(sum, 0);
				fprintf(fp, "%f, %f\n", sum, 0.0);
				index = (index+1) % chunk_size;
				
				if(index == 0){
					out->push(decimated);
					decimated = new complex<double>[chunk_size];
				}
			}
			counter = (counter+1) % dec_rate;
		}
	}
	fclose(fp);
	return nullptr;
}

void* pilot_extraction_thread_diffeq(void* args){
	string name = "PILOT EXTRACTION";
	// setup parameters 
	pilot_extract_args* params = (pilot_extract_args*) args;
	BlockingQueue<double>* in = params->in;
	BlockingQueue<complex<double>>* out = params->out;

	int Fs = params->sample_rate;
	int chunk_size = params->chunk_size;
	int dec_rate = params->dec_rate;
	int taps = params->taps;

	vector<complex<float>>* a;
	vector<complex<float>>* b;
	a = read_complex_coeffs(params->filter_path_diffeq_a);
	b = read_complex_coeffs(params->filter_path_diffeq_b);

	printf("[%s]	initialized filter difference equation\n", name.c_str());

	Deque<double> x_hist(b->size());
	Deque<double> y_hist(a->size()-1);

	complex<float> a0 = (*a)[0];
	int counter = 0;
	int index = 0;

	complex<double>* decimated = new complex<double>[chunk_size];

	double* sig_filtered = new double[chunk_size];

	FILE* fp;
	fp = fopen("output/exp/pilot.txt", "w");

	while(true){
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
			for(int coeff_index = 0; coeff_index < b->size(); coeff_index++){
				x_sum+=temp->data*(*b)[coeff_index].real();
				temp = temp->next;
			}

            temp = y_hist.head;
            for(int coeff_index = 1; coeff_index < a->size(); coeff_index++){
                y_sum += temp->data * (*a)[coeff_index].real();
                temp = temp->next;
            }
			double res = (x_sum-y_sum)/a0.real();
			y_hist.push_front(res);

			sig_filtered[i] = res;

			if(counter == 0){
                decimated[index] = complex<double>(sig_filtered[i]*sig_filtered[i], 0.0);
				fprintf(fp, "%f,%f\n", decimated[index].real(), decimated[index].imag());
                index = (index+1) % chunk_size;
                if(index == 0){
                    out->push(decimated);
                    decimated = new complex<double>[chunk_size];
                }
            }
            counter = (counter+1) % dec_rate;
		}
	}

	fclose(fp);
	return nullptr;
}