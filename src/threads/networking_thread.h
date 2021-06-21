#ifndef NETWORKING_THREAD_H
#define NETWORKING_THREAD_H

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>
#include <iostream>

#include "../utils/BlockingQueue.h"

struct networking_args{
	BlockingQueue<float>* in;
	int socket_fd;	
	int chunk_size;
};

void* networking_thread(void* args);

#endif