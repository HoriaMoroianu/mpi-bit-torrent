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
    string filename;
    vector<int> swarm;
    vector<ClientType> client_types;
    vector<string> segments;
};

extern MPI_Datatype MPI_FILE_DATA;

void Tracker(int numtasks, int rank);
void RecvClientFiles(int numtasks, unordered_map<string, TrackerData> &database);
void PrintDatabase(unordered_map<string, TrackerData> &database);

#endif
