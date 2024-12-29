// Copyright (c) 2024 Horia-Valentin MOROIANU

#ifndef __TRACKER_HPP__
#define __TRACKER_HPP__

#include "utils.hpp"
#include <mpi.h>
#include <pthread.h>
#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

enum ClientType{
    SEED,
    PEER,
    LEECH
};

struct TrackerData {
    FileData file;
    vector<int> swarm;
    vector<ClientType> client_types;
};

extern MPI_Datatype MPI_FILE_DATA;
extern MPI_Datatype MPI_SWARM_DATA;

void Tracker(int numtasks, int rank);
void RecvClientFiles(int numtasks, unordered_map<string, TrackerData> &database);
void FileRequest(unordered_map<string, TrackerData> &database, int source);
void FileComplete(unordered_map<string, TrackerData> &database, int source);

void PrintDatabase(unordered_map<string, TrackerData> &database);

#endif
