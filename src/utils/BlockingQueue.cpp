// #include <cstdio>
// #include <pthread.h>
// #include <iostream>
// #include <unistd.h>

// #include "BlockingQueue.h"

// using namespace std;

// template <class T>
// BlockingQueue<T>::BlockingQueue(int capacity){
//     this->head = nullptr;
//     this->tail = head;
//     this->size = 0;
//     this->capacity = capacity;
    
//     pthread_cond_init(&non_empty_cond, NULL);
//     pthread_mutex_init(&access_mutex, NULL);
// }

// template <class T>
// QueueElement<T>* BlockingQueue<T>::pop(){
//     pthread_mutex_lock(&access_mutex);
//     while(this->size == 0){
//         // cout<<"Blocking"<<endl;
//         pthread_cond_wait(&non_empty_cond, &access_mutex);
//     }

//     // the blocking is done
//     QueueElement<T>* popped = this->head;
//     if(!(popped)){
//         cout<<"bad"<<endl;
//     }

//     // adjust size
//     this->size--;

//     // if the queue is empty again, set head and tail to nullptr
//     if(this->size == 0) {
//         this->head = nullptr;
//         this->tail = head;
//     }else{
//     // otherwise move head to head->next
//         this->head = head->next;
//     }
//     pthread_mutex_unlock(&access_mutex);
//     return popped;
// }

// template <class T>
// void BlockingQueue<T>::push(T* buffer){
//     pthread_mutex_lock(&access_mutex);

//     QueueElement<T>* e = new QueueElement<T>();
//     e->data = buffer;
//     e->next = nullptr;

//     if(this->head == nullptr){
//         this->head = e;
//         this->tail = e;
//         this->size = 1;
//         pthread_cond_signal(&non_empty_cond);
//     }else{
//         this->tail->next = e;
//         this->tail = this->tail->next;
//         this->size++;
//     }
//     // cout<<e->data[0]<<endl;

//     // cout<<"+"<<this->size<<endl; 
//     pthread_mutex_unlock(&access_mutex);
// }

// template <class T>
// int BlockingQueue<T>::getsize(){
//     pthread_mutex_lock(&access_mutex);
//     int size = this->size;
//     pthread_mutex_unlock(&access_mutex);
//     return size;
// }

// template <class T>
// BlockingQueue<T>::~BlockingQueue(){
//     //stub
// }



