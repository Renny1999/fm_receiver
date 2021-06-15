#ifndef CAPTURE_THREAD_H
#define CAPTURE_THREAD_H

#include "../utils/BlockingQueue.h"

struct capture_args{
    double sample_rate;
    double center_freq;
    int chunk_size = 1024;
    BlockingQueue* out; 
};
void* capture_thread(void* args);       // captures IQ data and passes it to the lpfmc thread

#endif