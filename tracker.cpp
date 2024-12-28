// Copyright (c) 2024 Horia-Valentin MOROIANU

#include "tracker.hpp"
#include "peer.hpp"
#include "utils.hpp"

void Tracker(int numtasks, int rank)
{
    unordered_map<string, vector<int>> swarms; // (filename, list of peers)

    cout << "Tracker started\n";
    for (int i = 1; i < numtasks; i++) {
        vector<FileData> files(MAX_FILES);
        MPI_Status status;
        MPI_Recv(files.data(), MAX_FILES, MPI_FILE_DATA, i, 0, MPI_COMM_WORLD, &status);

        int num_files;
        MPI_Get_count(&status, MPI_FILE_DATA, &num_files);
        // TODO: check num_files?

        cout << num_files << " files received from peer " << i << '\n';

        for (int j = 0; j < num_files; j++) {
            swarms[files[j].filename].push_back(i);
        }
    }

    for (auto &[filename, peers] : swarms) {
        cout << filename << ": ";
        for (int peer : peers) {
            cout << peer << ' ';
        }
        cout << '\n';
    }
}
