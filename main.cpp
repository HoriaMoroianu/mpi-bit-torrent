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

    // New MPI datatype for client->tracker input files
    {
        int blocklengths[3] = { MAX_FILENAME, 1, MAX_CHUNKS * HASH_SIZE };
        MPI_Datatype types[3] = { MPI_CHAR, MPI_INT, MPI_CHAR };
        
        MPI_Aint displacements[3];
        displacements[0] = offsetof(FileData, name);
        displacements[1] = offsetof(FileData, segment_count);
        displacements[2] = offsetof(FileData, segments);

        MPI_Type_create_struct(3, blocklengths, displacements, types, &MPI_FILE_DATA);
        MPI_Type_commit(&MPI_FILE_DATA);
    }

    // New MPI datatype for tracker->client file
    {
        int blocklengths[3] = { 1, 1, MAX_CLIENTS };
        MPI_Datatype types[3] = { MPI_FILE_DATA, MPI_INT, MPI_INT };

        MPI_Aint displacements[3];
        displacements[0] = offsetof(SwarmData, file);
        displacements[1] = offsetof(SwarmData, swarm_size);
        displacements[2] = offsetof(SwarmData, swarm);

        MPI_Type_create_struct(3, blocklengths, displacements, types, &MPI_SWARM_DATA);
        MPI_Type_commit(&MPI_SWARM_DATA);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == TRACKER_RANK) {
        Tracker(numtasks, rank);
    } else {
        Client(numtasks, rank);
    }

    MPI_Type_free(&MPI_FILE_DATA);
    MPI_Type_free(&MPI_SWARM_DATA);
    MPI_Finalize();
}
