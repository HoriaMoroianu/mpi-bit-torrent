// Copyright (c) 2024 Horia-Valentin MOROIANU

#ifndef __PEER_HPP__
#define __PEER_HPP__

#include <mpi.h>
#include <pthread.h>
#include <iostream>

using namespace std;

void peer(int numtasks, int rank);
void *download_thread_func(void *arg);
void *upload_thread_func(void *arg);

#endif
