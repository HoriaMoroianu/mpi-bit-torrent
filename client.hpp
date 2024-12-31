// Copyright (c) 2024 Horia-Valentin MOROIANU

#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include "utils.hpp"
#include <mpi.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>

using namespace std;

extern MPI_Datatype MPI_FILE_DATA;
extern MPI_Datatype MPI_SWARM_DATA;

void Client(int numtasks, int rank);
void ReadInput(int rank, unordered_map<string, FileData> &owned_files,
               vector<string> &wanted_filenames);
void SendFilesToTracker(unordered_map<string, FileData> &owned_files);

void *DownloadThread(void *arg);
void *UploadThread(void *arg);

#endif
