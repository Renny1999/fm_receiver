#include <cstdio>
#include <iostream>
#include <vector>

#include <fftw3.h>
#include <string>
#include "../utils/utils.h"
#include "../utils/FixedSizedDeque.h"
#include "mono_audio_extraction_thread.h"

using namespace std;

void* mono_audio_extraction_thread_diffeq(void* args){
    string name = "MONO EXTRACT";
	// extract thread configs
	m_audio_extract_args* params = (m_audio_extract_args*) args;
	
	BlockingQueue<double>* in = params->in;
	BlockingQueue<double>* out = params->out;

	double Fs = params->sample_rate;
    double audio_freq = 48000;

    int dec_rate = params->dec_rate;

	int chunk_size = params->chunk_size;

    vector<float>* a;
    vector<float>* b;

    // read in diffeq "a" coefficients 
    a = read_butterworth_float_coeffs(params->filter_path_diffeq_a);

    // read in diffeq "b" coefficients 
    b = read_butterworth_float_coeffs(params->filter_path_diffeq_b);

    printf("[MONO EXTRACT]   initialized filter difference eqution\n");

    FixedSizedDeque<double> x_hist(b->size());  // all initialized to 0, x[n] ~ x[n-(b.size()-1)]
    FixedSizedDeque<double> y_hist(a->size()-1);  // all initialized to 0, does not include y[n-1] ~
    float a0 = (*a)[0];

    printf("[MONO EXTRACT]   a0 = %f\n", a0);

    int counter = 0;    // every dec_rate sample, the sample is saved
    int index = 0;      // goes from 0 to chunk_size-1

    // while(true){
    double* extracted = new double[chunk_size];
    for(int i = 0; i < 1000*8; i++){
        QueueElement<double>* popped = in->pop(3000);
        if(popped == nullptr){
            cout<<"[MONO EXTRACT]   time out!"<<endl;
            return nullptr;
        }
        double* data = popped->data;

        // if(data == nullptr){
        //     cout<<"[MONO EXTRACT]   time out!"<<endl;
        //     return nullptr;
        // }
        // apply filter here

        double* sig_filtered = new double[chunk_size];

        for(int data_index = 0; data_index < chunk_size; data_index++){
            double d = data[data_index];
            double x_sum = 0;
            double y_sum = 0;

            // calculate x_sum
            x_hist.push_front(d);   // add x[n] to the hist
            for(int coeff_index = 0; coeff_index < b->size(); coeff_index++){
                x_sum += x_hist[coeff_index] * (*b)[coeff_index];
            }

            // calculate y_sum
            for(int coeff_index = 1; coeff_index < a->size(); coeff_index++){
                y_sum += y_hist[coeff_index-1] * (*a)[coeff_index];
            }

            double res = (x_sum-y_sum)/a0;
            y_hist.push_front(res);

            sig_filtered[data_index] = res;
        }

        for(int j = 0; j < chunk_size; j++){
            if(counter == 0){
                extracted[index] = sig_filtered[j];

                index = (index+1) % chunk_size;

                if(index == 0){
                    out->push(extracted);
                    extracted = new double[chunk_size];
                }
            }
            counter = (counter+1) % dec_rate;
        }

        // contents of data no longer needed
        delete []data;
    }

    return nullptr;
}