// Copyright (c) 2024 Horia-Valentin MOROIANU

#ifndef __PEER_HPP__
#define __PEER_HPP__

#include <mpi.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

void Peer(int numtasks, int rank);
void ReadInput(int rank, vector<pair<string, vector<string>>> &files,
               vector<string> &wanted_filenames);

void *download_thread_func(void *arg);
void *upload_thread_func(void *arg);

#endif
