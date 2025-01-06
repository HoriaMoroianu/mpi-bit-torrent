// Copyright (c) 2024 Horia-Valentin MOROIANU

#ifndef __MAIN_HPP__
#define __MAIN_HPP__

#include <mpi.h>
#include <iostream>

using namespace std;

MPI_Datatype MPI_FILE_DATA;
MPI_Datatype MPI_SEGMENT;

void MPI_Create_File_Data(void);
void MPI_Create_Segment(void);

#endif
