#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

#include <complex>

struct QueueElement{
    public:
    std::complex<float>* data;
    QueueElement* next;
};

class BlockingQueue{
    public:
    QueueElement* head;
    QueueElement* tail;
    int size;
    pthread_mutex_t tail_lock;
    pthread_mutex_t head_lock;
    pthread_cond_t non_empty_cond;   // use this when the queue is empty

    QueueElement* pop();
    void push(std::complex<float>* buffer);

    int getsize();

    BlockingQueue();
    ~BlockingQueue();

};

#endif