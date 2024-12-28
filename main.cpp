// Copyright (c) 2024 Horia-Valentin MOROIANU

#include "main.hpp"
#include "utils.hpp"
#include "tracker.hpp"
#include "peer.hpp"

int main(int argc, char *argv[]) {
    int numtasks, rank;
 
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    DIE(provided < MPI_THREAD_MULTIPLE,
        "MPI nu are suport pentru multi-threading");

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == TRACKER_RANK) {
        tracker(numtasks, rank);
    } else {
        Peer(numtasks, rank);
    }

    MPI_Finalize();
}
