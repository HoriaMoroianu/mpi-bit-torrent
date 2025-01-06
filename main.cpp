// Copyright (c) 2024 Horia-Valentin MOROIANU

#include "main.hpp"
#include "utils.hpp"
#include "tracker.hpp"
#include "client.hpp"

int main(int argc, char *argv[]) {
    int numtasks, rank;

    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    DIE(provided < MPI_THREAD_MULTIPLE,
        "MPI nu are suport pentru multi-threading");

    // New MPI datatype for client<->tracker input files data
    MPI_Create_File_Data();

    // New MPI datatype for client<->client segment transfer
    MPI_Create_Segment();

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == TRACKER_RANK) {
        Tracker(numtasks);
    } else {
        Client(rank);
    }

    MPI_Type_free(&MPI_FILE_DATA);
    MPI_Type_free(&MPI_SEGMENT);
    MPI_Finalize();
}

void MPI_Create_File_Data(void)
{
    int blocklengths[3] = { MAX_FILENAME, 1, MAX_CHUNKS * HASH_SIZE };
    MPI_Datatype types[3] = { MPI_CHAR, MPI_INT, MPI_CHAR };
    
    MPI_Aint displacements[3];
    displacements[0] = offsetof(FileData, name);
    displacements[1] = offsetof(FileData, segment_count);
    displacements[2] = offsetof(FileData, segments);

    MPI_Type_create_struct(3, blocklengths, displacements, types,
                           &MPI_FILE_DATA);
    MPI_Type_commit(&MPI_FILE_DATA);
}

void MPI_Create_Segment(void)
{
    int blocklengths[3] = { MAX_FILENAME, 1, HASH_SIZE };
    MPI_Datatype types[3] = { MPI_CHAR, MPI_INT, MPI_CHAR };

    MPI_Aint displacements[3];
    displacements[0] = offsetof(DownloadSegment, filename);
    displacements[1] = offsetof(DownloadSegment, id);
    displacements[2] = offsetof(DownloadSegment, hash);

    MPI_Type_create_struct(3, blocklengths, displacements, types, &MPI_SEGMENT);
    MPI_Type_commit(&MPI_SEGMENT);
}
