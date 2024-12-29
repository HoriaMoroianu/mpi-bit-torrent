// Copyright (c) 2024 Horia-Valentin MOROIANU

#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include "utils.hpp"
#include <mpi.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

extern MPI_Datatype MPI_FILE_DATA;

void Client(int numtasks, int rank);
void ReadInput(int rank, vector<pair<string, vector<string>>> &files,
               vector<string> &wanted_filenames);
void SendFilesToTracker(vector<pair<string, vector<string>>> &files);

void *download_thread_func(void *arg);
void *upload_thread_func(void *arg);

#endif
