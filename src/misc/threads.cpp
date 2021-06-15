#include "threads.h"
#include "BlockingQueue.h"
#include <cstdio>
#include <cstdlib>
#include <complex>
#include <iostream>
#include <unistd.h> // sleep()


using namespace std;

// pure testing purpose
void* consume_thread(void* args){
    BlockingQueue* queue = (BlockingQueue*) args;
    while(true){
        queue->pop();
    }
}

void* supply_thread(void* args){
    BlockingQueue* queue = (BlockingQueue*) args;
    while(true){
        sleep(1.0/(2e6));
        int temp;
        complex<float>* buffer = nullptr;
        queue->push(buffer);
    }
}