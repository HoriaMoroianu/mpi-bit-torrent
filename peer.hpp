// Copyright (c) 2024 Horia-Valentin MOROIANU

#ifndef __PEER_HPP__
#define __PEER_HPP__

#include "utils.hpp"
#include <mpi.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

struct FileData {
    char filename[MAX_FILENAME];
    int segment_count;
    char segments[MAX_CHUNKS][HASH_SIZE];
};

extern MPI_Datatype MPI_FILE_DATA;

void Peer(int numtasks, int rank);
void ReadInput(int rank, vector<pair<string, vector<string>>> &files,
               vector<string> &wanted_filenames);
void SendFilesToTracker(vector<pair<string, vector<string>>> &files);

void *download_thread_func(void *arg);
void *upload_thread_func(void *arg);

#endif
