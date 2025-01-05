// Copyright (c) 2024 Horia-Valentin MOROIANU

#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include "utils.hpp"
#include <mpi.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

struct DownloadArgs {
    unordered_map<string, FileData> *owned_files;
    vector<string> *wanted_filenames;
    pthread_mutex_t *lock;
    int rank;
};

struct UploadArgs {
    unordered_map<string, FileData> *owned_files;
    pthread_mutex_t *lock;
    int rank;
};

extern MPI_Datatype MPI_FILE_DATA;
extern MPI_Datatype MPI_SEGMENT;

void Client(int rank);
void TransferFiles(unordered_map<string, FileData> &owned_files,
                   vector<string> &wanted_filenames, int rank);

void ReadInput(int rank, unordered_map<string, FileData> &owned_files,
               vector<string> &wanted_filenames);
void SendFilesToTracker(unordered_map<string, FileData> &owned_files);

void *DownloadThread(void *arg);
FileData RequestFile(string &filename);
vector<int> RequestSwarm(string &filename);
void UpdateSwarm(vector<int> &local_swarm, string &filename);
DownloadSegment RequestSegment(char *filename, int id, int peer);

void *UploadThread(void *arg);
void SendSegment(UploadArgs *args, int peer);

void SaveFile(FileData &file, string &filename, int rank);

#endif
