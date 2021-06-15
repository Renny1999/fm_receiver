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

#include <fstream>

using namespace std;

void* stage_1_filtering_thread(void* args){
    stage_1_filter_args* params = (stage_1_filter_args*) args;

    // set up parameters
    int taps = params->ntaps;
    int chunk_size = params->chunk_size;
    double Fs = params->sample_rate;
    BlockingQueue* in  = params->in;
    BlockingQueue* out  = params->out;

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
    file.open(params->filter_path);
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
            if(counter%dec_rate == 0){
                if(counter == dec_rate){
                    counter = 0;
                }

                sig_filtered[index] = inverse_fft_out[i];
                index++;
                if(index == chunk_size){
                    out->push(sig_filtered);
                    sig_filtered = new complex<float>[chunk_size];
                    index = 0;
                }
            }
            counter++;
        }

        delete []data;
    }
}