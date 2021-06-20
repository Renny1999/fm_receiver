#ifndef DECIMATION_THREAD
#define DECIMATION_THREAD

#include <complex>

#include "../utils/BlockingQueue.h"

struct decimation_args{
	
};
// for(int i = 0; i < chunk_size; i++){
//     if(counter == 0){      // if the sample is the one to be taken
//         decimated[index] = filtered[i];
//         // the next index at which sample will be stored
//         index = (index+1) % chunk_size;
//         if(index == 0){    // if the next index is 0, this buffer is full, send to queue
//         }
//     }
//     counter = (counter+1) % dec_rate;
// }
#endif