#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

#include <complex>
#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <string>

using namespace std;
/*
    Right now, BlockingQueue<T> will take in a pointer to T as the data stored in the QueueElement
    The QueueElement is supposed to contain a pointer to a buffer, not a single primitive 

    By taking in buffer, we can ensure that the destructor functions properly by calling delete []

    I feel bad for making it this way; maybe I will improve it in the future
*/
template <class T>
struct QueueElement{
    // std::complex<float>* data;
    // QueueElement* next;
    T* data;
    QueueElement* next;

    ~QueueElement(){
        if(this->data){
            delete [](this->data);
        }
        this->next = nullptr;
    }
};

template <class T>
class BlockingQueue{
    public:
    QueueElement<T>* head;
    QueueElement<T>* tail;
    int size;
    int capacity;
    pthread_mutex_t access_mutex;
    pthread_cond_t non_empty_cond;   // use this when the queue is empty

    QueueElement<T>* pop(std::string owner = "");
    QueueElement<T>* pop(int timeout, std::string owner = "");
    // void push(std::complex<float>* buffer);
    void push(T* buffer);

    int getsize();

    BlockingQueue(int capacity = 400);
    ~BlockingQueue();
}; 

template <class T>
BlockingQueue<T>::BlockingQueue(int capacity){
    this->head = nullptr;
    this->tail = head;
    this->size = 0;
    this->capacity = capacity;
    
    pthread_cond_init(&non_empty_cond, NULL);
    pthread_mutex_init(&access_mutex, NULL);
}

template <class T>
QueueElement<T>* BlockingQueue<T>::pop(std::string owner){
    pthread_mutex_lock(&access_mutex);
    while(this->size == 0){
        // printf("[%s]    blocking\n", owner.c_str());
        pthread_cond_wait(&non_empty_cond, &access_mutex);
    }

    // the blocking is done
    QueueElement<T>* popped = this->head;
    if(!(popped)){
        std::cout<<"bad"<<std::endl;
    }
    popped->next = nullptr;

    // adjust size
    this->size--;

    // if the queue is empty again, set head and tail to nullptr
    if(this->size == 0) {
        this->head = nullptr;
        this->tail = head;
    }else{
    // otherwise move head to head->next
        this->head = head->next;
    }
    pthread_mutex_unlock(&access_mutex);
    return popped;
}

// times out after timeInMs if the queue is empty. returns nullptr
template <class T>
QueueElement<T>* BlockingQueue<T>::pop(int timeInMs, std::string name){

    struct timeval tv;
    struct timespec ts;

    gettimeofday(&tv, NULL);
    ts.tv_sec = time(NULL)+timeInMs/1000;
    ts.tv_nsec = tv.tv_usec * 1000 + 1000 * 1000 * (timeInMs % 1000);
    ts.tv_sec += ts.tv_nsec / (1000 * 1000 * 1000);
    ts.tv_nsec %= (1000 * 1000 * 1000);

    pthread_mutex_lock(&access_mutex);
    while(this->size == 0){
        // printf("[%s]    Blocking\n", name.c_str());
        int res = pthread_cond_timedwait(&non_empty_cond, &access_mutex, &ts);
        if(res == ETIMEDOUT){
            return nullptr;
        }
    }

    // the blocking is done
    QueueElement<T>* popped = this->head;
    if(!(popped)){
        std::cout<<"bad"<<std::endl;
    }

    // adjust size
    this->size--;

    // if the queue is empty again, set head and tail to nullptr
    if(this->size == 0) {
        this->head = nullptr;
        this->tail = head;
    }else{
    // otherwise move head to head->next
        this->head = head->next;
    }
    pthread_mutex_unlock(&access_mutex);
    return popped;
}

template <class T>
void BlockingQueue<T>::push(T* buffer){
    pthread_mutex_lock(&access_mutex);

    QueueElement<T>* e = new QueueElement<T>();
    e->data = buffer;
    e->next = nullptr;

    if(this->head == nullptr){
        this->head = e;
        this->tail = e;
        this->size = 1;
        pthread_cond_signal(&non_empty_cond);
    }else{
        this->tail->next = e;
        this->tail = this->tail->next;
        this->size++;
    }
    // cout<<e->data[0]<<endl;

    // cout<<"+"<<this->size<<endl; 
    pthread_mutex_unlock(&access_mutex);
}

template <class T>
int BlockingQueue<T>::getsize(){
    pthread_mutex_lock(&access_mutex);
    int size = this->size;
    pthread_mutex_unlock(&access_mutex);
    return size;
}

template <class T>
BlockingQueue<T>::~BlockingQueue(){
    if(this->head){
        delete this->head;
    }
    if(this->tail){
        delete this->tail;
    }
}


#endif