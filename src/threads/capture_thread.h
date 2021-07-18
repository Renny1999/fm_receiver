#ifndef CAPTURE_THREAD_H
#define CAPTURE_THREAD_H

#include <complex>
#include <atomic>

#include "../utils/BlockingQueue.h"

struct capture_args{
    std::atomic<bool>* exit_loop;
    double sample_rate;
    double center_freq;
    int chunk_size = 1024;
    BlockingQueue<std::complex<float>>* out; 
};
void* capture_thread(void* args);       // captures IQ data and passes it to the lpfmc thread

#endif