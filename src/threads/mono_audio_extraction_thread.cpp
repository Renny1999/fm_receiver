#include <cstdio>
#include <iostream>

#include <fftw3.h>

#include "mono_audio_extraction_thread.h"

using namespace std;

void* mono_audio_extraction_thread(void* args){
	// extract thread configs
	m_audio_extract_args* params = (m_audio_extract_args*) args;
	
	BlockingQueue<double>* in = params->in;
	BlockingQueue<double>* out = params->out;

	double Fs = params->sample_rate;
	int chunk_size = params->chunk_size;
	string filter_path = params->filter_path_fft;

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

	
}