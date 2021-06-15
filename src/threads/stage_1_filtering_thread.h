#ifndef STATE_1_FILTERING_THREAD_H
#define STATE_1_FILTERING_THREAD_H

#include <string>
#include "../utils/BlockingQueue.h"

struct stage_1_filter_args{
    BlockingQueue* in; 
    BlockingQueue* out; 
    std::string filter_path;
    int ntaps;
    int chunk_size;
};

void* stage_1_filtering_thread(void* args);

#endif