#include <cstdio>
#include <cstdlib>
#include <complex>
#include <iostream>
#include <unistd.h> // sleep()
#include <string>
#include <vector>
#include <deque>
#include <chrono>

#include <fftw3.h>


#include "stage_1_filtering_thread.h"
#include "../utils/utils.h"
#include "../utils/FixedSizedDeque.h"
#include "../utils/Deque.h"

#include <fstream>

using namespace std;
using namespace chrono;

// performs filtering using the fft method with remez filter
void* stage_1_filtering_thread_fft(void* args){
    stage_1_filtering_args* params = (stage_1_filtering_args*) args;

    // set up parameters
    int taps = params->ntaps;
    int chunk_size = params->chunk_size;
    double Fs = params->sample_rate;
    BlockingQueue<complex<float>>* in  = params->in;
    BlockingQueue<complex<float>>* out  = params->out;

    int dec_rate = params->dec_rate;

    // setup fftw parameters
    complex<float>* forward_fft_in = new complex<float>[512];
    complex<float>* forward_fft_out = new complex<float>[512];
    complex<float>* inverse_fft_in = new complex<float>[512];
    complex<float>* inverse_fft_out = new complex<float>[512];

    fftwf_plan forward_plan;
    forward_plan = fftwf_plan_dft_1d(
        512, 
        reinterpret_cast<fftwf_complex*>(forward_fft_in), 
        reinterpret_cast<fftwf_complex*>(forward_fft_out),
        FFTW_FORWARD, 
        FFTW_ESTIMATE);

    fftwf_plan inverse_plan;
    inverse_plan = fftwf_plan_dft_1d(
        512, 
        reinterpret_cast<fftwf_complex*>(inverse_fft_in), 
        reinterpret_cast<fftwf_complex*>(inverse_fft_out),
        FFTW_BACKWARD, 
        FFTW_ESTIMATE);

    // read in filter coefficients
    ifstream file;
    file.open(params->filter_path_fft);
    complex<float>* filter_fft = new complex<float>[chunk_size];
    if(file.is_open()){
        string temp;
        for(int i = 0; i < chunk_size; i++){
            getline(file, temp);
            cout<<temp<<endl;
            float real_part = stof(temp)/512;
            getline(file, temp);
            cout<<temp<<endl;
            float imag_part = stof(temp)/512;
            filter_fft[i] = complex<float>(real_part, imag_part);
        }
        file.close();
    }else{
        printf("[STAGE 1]   Failed to read filter FFT\n");
        exit(-1);
    }

    printf("[STAGE 1]   initialized filter FFT\n");

    int counter = 0;    // every dec_rate sample, the sample is saved
    int index = 0;      // goes from 0 to chunk_size-1
    // while(true){
    
    // this is also the decimated signal
    complex<float>* sig_filtered = new complex<float>[chunk_size];
    for(int i = 0; i < 5000*8; i++){
        printf("[STAGE 1]   %d\n", i);
        complex<float>* data = in->pop()->data;

        complex<float>* fft_res = new complex<float>[chunk_size];   // stores the result of the FFT
        complex<float>* filtered = new complex<float>[chunk_size];  // stores the filtered signal 

        // perform FFT on the signal
        // cout<<"performing FFT"<<endl;

        // copy data input data into the buffer:
        for(int data_index = 0; data_index < chunk_size; data_index++){
            forward_fft_in[data_index] = data[data_index];
        }
        fftwf_execute(forward_plan);

        // filtering in the Fourier domain
        // copy the filtered spectrum into the buffer

        for(int data_index = 0; data_index < chunk_size; data_index++){
            complex<float> product = forward_fft_out[data_index]*filter_fft[data_index];
            // think about scaling the iFFT result
            // maybe it's not needed due to scaling factor introduced by the filter?
            inverse_fft_in[data_index] = complex<float>(product.real(), product.imag());
        }

        // perform inverse FFT
        fftwf_execute(inverse_plan);

        // now, the correct data is pointed by inverse_fft_out
        // copy the result to a seperate buffer
        //      *   maybe we can also perform decimation here


        for(int data_index = 0; data_index < chunk_size; data_index++){

            complex<float> d = inverse_fft_out[data_index];

            if(counter == 0){      // if the sample is the one to be taken
                sig_filtered[index] = inverse_fft_out[data_index];
                // the next index at which sample will be stored
                index = (index+1) % chunk_size;
                if(index == 0){    // if the next index is 0, this buffer is full, send to queue
                    out->push(sig_filtered);
                    sig_filtered = new complex<float>[chunk_size];
                }
            }
            counter = (counter+1) % dec_rate;
        }

        
        delete []data;
    }
    return nullptr;
}

// performs filtering using butterworth filter difference equations
void* stage_1_filtering_thread_diffeq_ll(void* args){
    string name = "STAGE 1";

    stage_1_filtering_args* params = (stage_1_filtering_args*) args;

    // set up parameters
    int taps = params->ntaps;
    int chunk_size = params->chunk_size;
    double Fs = params->sample_rate;
    BlockingQueue<complex<float>>* in  = params->in;
    BlockingQueue<complex<float>>* out  = params->out;

    int dec_rate = params->dec_rate;

    vector<complex<float>>* a;
    vector<complex<float>>* b;

    // read in diffeq "a" coefficients 
    a = read_complex_float_coeffs(params->filter_path_diffeq_a);

    // read in diffeq "b" coefficients 
    b = read_complex_float_coeffs(params->filter_path_diffeq_b);

    printf("[STAGE 1]   initialized filter difference equation\n");


    Deque<complex<float>> x_hist(b->size());  // all initialized to 0, x[n] ~ x[n-(b.size()-1)]
    Deque<complex<float>> y_hist(a->size()-1);  // all initialized to 0, does not include y[n-1] ~

    complex<float> a0 = (*a)[0];

    printf("[STAGE 1]   a0 = (%f,%f)\n", a0.real(), a0.imag());

    int counter = 0;    // every dec_rate sample, the sample is saved
    int index = 0;      // goes from 0 to chunk_size-1

    // while(true){
    complex<float>* decimated = new complex<float>[chunk_size];

    for(int i = 0; i < 5000*8; i++){
        printf("[STAGE 1]   %d\n", i);
        complex<float>* data = in->pop(name)->data;
        // apply filter here

        complex<float>* sig_filtered = new complex<float>[chunk_size];

        for(int data_index = 0; data_index < chunk_size; data_index++){
            complex<float> d = data[data_index];
            complex<float> x_sum(0.0,0.0);
            complex<float> y_sum(0.0,0.0);

            // calculate x_sum
            x_hist.push_front(d);   // add x[n] to the hist

            Node<complex<float>>* temp = x_hist.head;
            for(int coeff_index = 0; coeff_index < b->size(); coeff_index++){
                x_sum += temp->data * (*b)[coeff_index];
                temp = temp->next;
            }

            // calculate y_sum

            temp = y_hist.head;
            for(int coeff_index = 1; coeff_index < a->size(); coeff_index++){
                y_sum += temp->data * (*a)[coeff_index];
                temp = temp->next;
            }

            complex<float> res = (x_sum-y_sum)/a0;
            y_hist.push_front(res);
            sig_filtered[data_index] = res;

            if(counter == 0){
                decimated[index] = sig_filtered[data_index];
                index = (index+1) % chunk_size;
                if(index == 0){
                    out->push(decimated);
                    decimated = new complex<float>[chunk_size];
                }
            }
            counter = (counter+1) % dec_rate;
        }

        // contents of data no longer needed
        delete []data;

    }
    return nullptr;
}

// performs filtering using butterworth filter difference equations
void* stage_1_filtering_thread_diffeq_dq(void* args){
    string name = "STAGE 1";

    stage_1_filtering_args* params = (stage_1_filtering_args*) args;

    // set up parameters
    int taps = params->ntaps;
    int chunk_size = params->chunk_size;
    double Fs = params->sample_rate;
    BlockingQueue<complex<float>>* in  = params->in;
    BlockingQueue<complex<float>>* out  = params->out;

    int dec_rate = params->dec_rate;

    vector<complex<float>>* a;
    vector<complex<float>>* b;

    // read in diffeq "a" coefficients 
    a = read_complex_float_coeffs(params->filter_path_diffeq_a);

    // read in diffeq "b" coefficients 
    b = read_complex_float_coeffs(params->filter_path_diffeq_b);

    printf("[STAGE 1]   initialized filter difference eqution\n");


    FixedSizedDeque<complex<float>> x_hist(b->size());  // all initialized to 0, x[n] ~ x[n-(b.size()-1)]
    FixedSizedDeque<complex<float>> y_hist(a->size()-1);  // all initialized to 0, does not include y[n-1] ~

    complex<float> a0 = (*a)[0];

    printf("[STAGE 1]   a0 = (%f,%f)\n", a0.real(), a0.imag());

    int counter = 0;    // every dec_rate sample, the sample is saved
    int index = 0;      // goes from 0 to chunk_size-1

    // while(true){
    complex<float>* decimated = new complex<float>[chunk_size];

    for(int i = 0; i < 5000*8; i++){
        printf("[STAGE 1]   %d\n", i);
        complex<float>* data = in->pop(name)->data;
        // apply filter here

        complex<float>* sig_filtered = new complex<float>[chunk_size];

        for(int data_index = 0; data_index < chunk_size; data_index++){
            complex<float> d = data[data_index];
            complex<float> x_sum(0.0,0.0);
            complex<float> y_sum(0.0,0.0);

            // calculate x_sum
            x_hist.push_front(d);   // add x[n] to the hist
            
            for(int coeff_index = 0; coeff_index < b->size(); coeff_index++){
                x_sum += x_hist[coeff_index] * (*b)[coeff_index];
            }

            // calculate y_sum
            float real_party = 0;
            float imag_party = 0;
            for(int coeff_index = 1; coeff_index < a->size(); coeff_index++){
                y_sum += y_hist[coeff_index-1] * (*a)[coeff_index];
            }

            complex<float> res = (x_sum-y_sum)/a0;
            y_hist.push_front(res);
            sig_filtered[data_index] = res;

            if(counter == 0){
                decimated[index] = sig_filtered[data_index];
                index = (index+1) % chunk_size;
                if(index == 0){
                    out->push(decimated);
                    decimated = new complex<float>[chunk_size];
                }
            }
            counter = (counter+1) % dec_rate;
        }

        // contents of data no longer needed
        delete []data;

    }
    return nullptr;
}

void* stage_1_filtering_thread_h(void* args){
    string name = "STAGE 1";
    int c = 0;

    stage_1_filtering_args* params = (stage_1_filtering_args*) args;

    // set up parameters
    int taps = params->ntaps;
    int chunk_size = params->chunk_size;
    double Fs = params->sample_rate;
    BlockingQueue<complex<float>>* in  = params->in;
    BlockingQueue<complex<float>>* out  = params->out;

    int dec_rate = params->dec_rate;

    vector<complex<float>>* h = read_complex_float_coeffs(params->filter_path_h);
    printf("[%s]    finished setting up filter coeffs\n", name.c_str());
    for(int i = 0; i < h->size(); i++){
        cout<<(*h)[i]<<endl;
    }
    Deque<complex<float>> x_hist(taps); // x[n]->x[n-1]->x[n-2]->...x[n-taps+1]

    complex<float>* data;
    complex<float>* decimated = new complex<float>[chunk_size];

    int counter = 0;
    int index = 0;
    while(true){
        QueueElement<complex<float>>* popped = in->pop(3000, name);
        printf("[STAGE 1]   %d\n", c);
        c++;

        if(popped == nullptr){
            printf("[%s]    timed out!, exiting thread...\n", name.c_str());
            return nullptr;
        }

        data = popped->data;
        for(int i = 0; i < chunk_size; i++){
            x_hist.push_front(data[i]);
            // calculate y[n] only if it is the value needed
            if(counter == 0){
                // this sample has to be taken, so calculate y[n]
                complex<float> sum(0,0);
                Node<complex<float>>* temp = x_hist.head;

                for(int j = 0; j < taps; j++) {
                    sum += (*h)[j] * (temp->data);
                    temp = temp->next;
                }
                decimated[index] = sum;
                index = (index+1) % chunk_size;
                if(index == 0){
                    out->push(decimated);
                    decimated = new complex<float>[chunk_size];
                }
            }
            counter = (counter+1) % dec_rate;
        }
    }// end while
}// end stage_1_filtering_thread