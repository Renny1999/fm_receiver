#ifndef MONO_AUDIO_EXTRACTION_THREAD_H
#define MONO_AUDIO_EXTRACTION_THREAD_H

#include <complex>
#include <string>
#include "../utils/BlockingQueue.h"

struct m_audio_extract_args{
    BlockingQueue<double>* in; 
    BlockingQueue<double>* out; 
    std::string filter_path_fft;
    std::string filter_path_diffeq_a;
    std::string filter_path_diffeq_b;
    double sample_rate;
    int chunk_size;
    int signal_bw;
};

//  lowpass the signal at 0~200kHz, then decimate it to achieve a new frequency of 200kHz
void* mono_audio_extraction_thread_diffeq(void* args);

#endif