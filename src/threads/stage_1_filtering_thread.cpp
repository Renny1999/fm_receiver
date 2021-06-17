#include <cstdio>
#include <cstdlib>
#include <complex>
#include <iostream>
#include <unistd.h> // sleep()
#include <string>
#include <vector>
#include <deque>
#include <fftw3.h>

#include "stage_1_filtering_thread.h"
#include "../utils/utils.h"
#include "../utils/FixedSizedDeque.h"

#include <fstream>

using namespace std;

// performs filtering using the fft method with remez filter
void* stage_1_filtering_thread_fft(void* args){
    stage_1_filter_args* params = (stage_1_filter_args*) args;

    // set up parameters
    int taps = params->ntaps;
    int chunk_size = params->chunk_size;
    double Fs = params->sample_rate;
    BlockingQueue<complex<float>>* in  = params->in;
    BlockingQueue<complex<float>>* out  = params->out;

    int dec_rate = int(Fs/200000);

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
    // float* filter_coeffs = new float[taps];
    complex<float>* filter_fft = new complex<float>[chunk_size];
    if(file.is_open()){
        string temp;
        // for(int i = 0; i < taps; i++){
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
    complex<float>* sig_filtered = new complex<float>[chunk_size];

    for(int i = 0; i < 5000; i++){
        complex<float>* sig_filtered = new complex<float>[chunk_size];
        complex<float>* data = in->pop()->data;

        /*          approach 2      FFT       */
        complex<float>* fft_res = new complex<float>[chunk_size];   // stores the result of the FFT
        complex<float>* filtered = new complex<float>[chunk_size];  // stores the filtered signal 

        // perform FFT on the signal
        // cout<<"performing FFT"<<endl;

        // copy data input data into the buffer:
        for(int i = 0; i < chunk_size; i++){
            forward_fft_in[i] = data[i];
        }
        fftwf_execute(forward_plan);

        // filtering in the Fourier domain
        // copy the filtered spectrum into the buffer
        for(int i = 0; i < chunk_size; i++){
            complex<float> product = forward_fft_out[i]*filter_fft[i];
            // think about scaling the iFFT result
            // maybe it's not needed due to scaling factor introduced by the filter?
            inverse_fft_in[i] = complex<float>(product.real(), product.imag());
        }

        // perform inverse FFT
        fftwf_execute(inverse_plan);

        // now, the correct data is pointed by inverse_fft_out
        // copy the result to a seperate buffer
        //      *   maybe we can also perform decimation here


        for(int i = 0; i < chunk_size; i++){
            // if(counter%dec_rate == 0){
            //     if(counter == dec_rate){
            //         counter = 0;
            //     }

            //     sig_filtered[index] = inverse_fft_out[i];
            //     index++;
            //     if(index == chunk_size){
            //         out->push(sig_filtered);
            //         sig_filtered = new complex<float>[chunk_size];
            //         index = 0;
            //     }
            // }
            // counter++;
            sig_filtered[i] = inverse_fft_out[i];
        }
        out->push(sig_filtered);

        delete []data;
    }
}

// performs filtering using butterworth filter difference equations
void* stage_1_filtering_thread_diffeq(void* args){
    stage_1_filter_args* params = (stage_1_filter_args*) args;

    // set up parameters
    int taps = params->ntaps;
    int chunk_size = params->chunk_size;
    double Fs = params->sample_rate;
    BlockingQueue<complex<float>>* in  = params->in;
    BlockingQueue<complex<float>>* out  = params->out;

    int dec_rate = int(Fs/200000);

    vector<complex<float>>* a;
    vector<complex<float>>* b;

    // read in diffeq "a" coefficients 
    a = read_butterworth_coeffs(params->filter_path_diffeq_a);

    // read in diffeq "b" coefficients 
    b = read_butterworth_coeffs(params->filter_path_diffeq_b);
   
    printf("[STAGE 1]   initialized filter difference eqution\n");

    FixedSizedDeque<complex<float>> x_hist(b->size());  // all initialized to 0, x[n] ~ x[n-(b.size()-1)]
    FixedSizedDeque<complex<float>> y_hist(a->size()-1);  // all initialized to 0, does not include y[n-1] ~

    complex<float> a0 = (*a)[0];

    printf("[STAGE 1]   a0 = (%f,%f)\n", a0.real(), a0.imag());

    int counter = 0;    // every dec_rate sample, the sample is saved
    int index = 0;      // goes from 0 to chunk_size-1

    // while(true){


    for(int i = 0; i < 5000; i++){
        complex<float>* data = in->pop()->data;
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
            for(int coeff_index = 1; coeff_index < a->size(); coeff_index++){
                y_sum += y_hist[coeff_index-1] * (*a)[coeff_index];
            }

            complex<float> res = (x_sum-y_sum)/a0;
            y_hist.push_front(res);

            sig_filtered[data_index] = res;
        }

        complex<float>* decimated = new complex<float>[chunk_size];

        for(int i = 0; i < chunk_size; i++){
            if(counter%dec_rate == 0){
                if(counter == dec_rate){
                    counter = 0;
                }

                decimated[index] = sig_filtered[i];
                index++;
                if(index == chunk_size){
                    out->push(decimated);
                    decimated = new complex<float>[chunk_size];
                    index = 0;
                }
            }
            counter++;
        }

        // contents of data no longer needed
        delete []data;

    }
}
