#ifndef NETWORKING_THREAD_H
#define NETWORKING_THREAD_H

#include <iostream>
#include <atomic>

#include "../utils/BlockingQueue.h"

struct networking_args{
	std::atomic<bool>* exit_loop;
	BlockingQueue<double>* LRsum;
	BlockingQueue<double>* LRdiff;
	int socket_fd;	
	int chunk_size;
};

void* networking_thread(void* args);

#endif