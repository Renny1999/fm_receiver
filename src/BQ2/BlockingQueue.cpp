
#include "BlockingQueue.h"
#include <cstdio>
#include <pthread.h>
#include <iostream>

using namespace std;

BlockingQueue::BlockingQueue(){
    QueueElement* temp = new QueueElement();
    temp->data = nullptr;
    temp->next = nullptr;

    this->head = temp;
    this->tail = temp;

    this->size = 0;

    pthread_cond_init(&non_empty_cond, NULL);
    pthread_mutex_init(&head_lock, NULL);
    pthread_mutex_init(&tail_lock, NULL);
}

QueueElement* BlockingQueue::pop(){
    pthread_mutex_lock(&head_lock);
    while(this->size == 0){
        cout<<"blocking"<<endl;
        pthread_cond_wait(&non_empty_cond, &head_lock);
    }
    // cout<<"Done blocking: "<<(this->size)<<endl;

    // the blocking is done
    QueueElement* tmp = this->head;
    QueueElement* new_head = tmp->next;
    this->size--;

    QueueElement* res = new_head;
    this->head = new_head;
    // cout<<"-"<<this->size<<endl; 
    cout<<"removed"<<endl;
    if(tmp->data != nullptr){
        delete [](tmp->data);
        delete tmp;
    }
    pthread_mutex_unlock(&head_lock);
    return res;
}

void BlockingQueue::push(complex<float>* buffer){
    QueueElement* e = new QueueElement();
    e->data = buffer;
    e->next = nullptr;

    pthread_mutex_lock(&tail_lock);
    this->tail->next = e;
    this->tail = e;
    pthread_cond_signal(&non_empty_cond);
    this->size++;
    cout<<"addded"<<endl;

    // cout<<"+"<<this->size<<endl; 
    pthread_mutex_unlock(&tail_lock);
}

int BlockingQueue::getsize(){
    pthread_mutex_lock(&head_lock);
    pthread_mutex_lock(&tail_lock);
    int size = this->size;
    pthread_mutex_unlock(&head_lock);
    pthread_mutex_unlock(&tail_lock);
    return size;
}


BlockingQueue::~BlockingQueue(){
    //stub
}



