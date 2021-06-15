#ifndef THREADS_H_
#define THREADS_H_


// class BlockingQueue;

// struct capture_args{
//     double sample_rate;
//     double center_freq;
//     int chunk_size;
//     BlockingQueue* out; 
// };

// struct stage_1_filter_args{
//     BlockingQueue* in; 
//     BlockingQueue* out; 
//     string filter_path;
//     int ntaps;
// };

// void* capture_thread(void* args);       // captures IQ data and passes it to the lpfmc thread
// void* stage_1_filtering_thread(void* args);   // applies low pass filter to extract the FM bandwidth


void* supply_thread(void* args);
void* consume_thread(void* args);

#endif