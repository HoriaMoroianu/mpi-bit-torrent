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

void Tracker(int numtasks);
void RecvClientFiles(int numtasks, unordered_map<string, TrackerData> &database);
void SendFile(unordered_map<string, TrackerData> &database, int source);
void SendSwarm(unordered_map<string, TrackerData> &database, int source);
void FileComplete(unordered_map<string, TrackerData> &database, int source);

void PrintDatabase(unordered_map<string, TrackerData> &database);

#endif
