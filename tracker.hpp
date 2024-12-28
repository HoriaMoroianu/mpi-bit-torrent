// Copyright (c) 2024 Horia-Valentin MOROIANU

#ifndef __TRACKER_HPP__
#define __TRACKER_HPP__

#include <mpi.h>
#include <pthread.h>
#include <iostream>
#include <unordered_map>

using namespace std;

extern MPI_Datatype MPI_FILE_DATA;

void Tracker(int numtasks, int rank);

#endif
