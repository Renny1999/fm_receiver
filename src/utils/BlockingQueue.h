#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

#include <complex>

struct QueueElement{
    std::complex<float>* data;
    QueueElement* next;
};

class BlockingQueue{
    public:
    QueueElement* head;
    QueueElement* tail;
    int size;
    int capacity;
    pthread_mutex_t access_mutex;
    pthread_cond_t non_empty_cond;   // use this when the queue is empty

    QueueElement* pop();
    void push(std::complex<float>* buffer);

    int getsize();

    BlockingQueue(int capacity = 400);
    ~BlockingQueue();

};

#endif