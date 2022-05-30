#ifndef LOWPASS_THREAD
#define LOWPASS_THREAD

#include <complex>
#include <atomic>
#include "../utils/BlockingQueue.h"


#define stage_1_filtering_thread(args) (filtering_thread_h(void* args))

struct stage_1_filtering_args{
    std::atomic<bool>* exit_loop;
    BlockingQueue<std::complex<float>>* in; 
    BlockingQueue<std::complex<float>>* out; 
    std::string filter_path_h;
    int num_input;
    int num_output;
    double sample_rate;
    int ntaps;
    int chunk_size;
    int signal_bw;
};

//  lowpass the signal at 0~200kHz, then decimate it to achieve a new frequency of 200kHz
void* stage_1_filtering_thread_fft(void* args);

void* stage_1_filtering_thread_diffeq_dq(void* args);
void* stage_1_filtering_thread_diffeq_ll(void* args);
void* filtering_thread_h(void* args);

#endif
