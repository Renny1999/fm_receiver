#ifndef LOWPASS_THREAD
#define LOWPASS_THREAD

#include <complex>
#include <atomic>
#include "../utils/BlockingQueue.h"

struct stage_1_filtering_args{
    std::atomic<bool>* exit_loop;
    BlockingQueue<std::complex<float>>* in; 
    BlockingQueue<std::complex<float>>* out; 
    std::string filter_path_fft;
    std::string filter_path_h;
    std::string filter_path_diffeq_a;
    std::string filter_path_diffeq_b;
    double sample_rate;
    int ntaps;
    int chunk_size;
    int signal_bw;
    int dec_rate;
};

//  lowpass the signal at 0~200kHz, then decimate it to achieve a new frequency of 200kHz
void* stage_1_filtering_thread_fft(void* args);

void* stage_1_filtering_thread_diffeq_dq(void* args);
void* stage_1_filtering_thread_diffeq_ll(void* args);
void* stage_1_filtering_thread_h(void* args);

#endif