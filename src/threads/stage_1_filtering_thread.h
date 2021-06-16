#ifndef STATE_1_FILTERING_THREAD_H
#define STATE_1_FILTERING_THREAD_H

#include <string>
#include <complex>
#include "../utils/BlockingQueue.h"

struct stage_1_filter_args{
    BlockingQueue<std::complex<float>>* in; 
    BlockingQueue<std::complex<float>>* out; 
    std::string filter_path_fft;
    std::string filter_path_diffeq_a;
    std::string filter_path_diffeq_b;
    double sample_rate;
    int ntaps;
    int chunk_size;
};

//  lowpass the signal at 0~200kHz, then decimate it to achieve a new frequency of 200kHz
void* stage_1_filtering_thread_fft(void* args);

void* stage_1_filtering_thread_diffeq(void* args);

#endif