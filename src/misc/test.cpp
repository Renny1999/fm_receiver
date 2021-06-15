#include "utils/BlockingQueue.h"
#include "threads.h"
#include <pthread.h>
#include <cstdio>
#include <iostream>
#include <string>

using namespace std;

int main(){
    // make a blocking queue

    BlockingQueue bq;
    // bq.push(new complex<float>(1,1));
    // bq.push(new complex<float>(2,1));
    // bq.push(new complex<float>(3,1));
    // bq.push(new complex<float>(4,1));
    // bq.push(new complex<float>(5,1));
    // bq.push(new complex<float>(6,1));



    // cout<<*(bq.pop()->data)<<endl;
    // cout<<*(bq.pop()->data)<<endl;
    // cout<<*(bq.pop()->data)<<endl;
    // cout<<*(bq.pop()->data)<<endl;
    // cout<<*(bq.pop()->data)<<endl;
    // cout<<*(bq.pop()->data)<<endl;

    pthread_t threadId1;
    pthread_create(&threadId1, NULL, &supply_thread, &bq);
    printf("[main]  supply thread created with ID: %d\n", threadId1);

    pthread_t threadId2;
    pthread_create(&threadId2, NULL, &consume_thread, &bq);
    printf("[main]  consume thread created with ID: %d\n", threadId2);

    pthread_join(threadId1, NULL);
    pthread_join(threadId2, NULL);

}