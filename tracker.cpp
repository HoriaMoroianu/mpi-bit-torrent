// Copyright (c) 2024 Horia-Valentin MOROIANU

#include "tracker.hpp"
#include "client.hpp"

void Tracker(int numtasks, int rank)
{
    cout << "Tracker started\n";
    unordered_map<string, TrackerData> database;
    RecvClientFiles(numtasks, database);
    
    PrintDatabase(database);
}

void RecvClientFiles(int numtasks, unordered_map<string, TrackerData> &database)
{
    MPI_Status status;

    for (int i = 1; i < numtasks; i++) {
        // Receive files from client i
        vector<FileData> recv_files(MAX_FILES);
        MPI_Recv(recv_files.data(), MAX_FILES, MPI_FILE_DATA, i, 0, MPI_COMM_WORLD, &status);

        int file_count;
        MPI_Get_count(&status, MPI_FILE_DATA, &file_count);
        recv_files.resize(file_count);

        // Save files in database
        for (auto &file : recv_files) {
            // Add client to swarm
            TrackerData &data = database[file.filename];
            data.swarm.push_back(i);
            data.client_types.push_back(ClientType::SEED);

            if (!data.segments.empty()) {
                continue;
            }

            // Add segments if file is new to database
            data.filename = file.filename;
            data.segments.reserve(file.segment_count);
            for (int j = 0; j < file.segment_count; j++) {
                data.segments.push_back(file.segments[j]);
            }
        }
    }
}

void PrintDatabase(unordered_map<string, TrackerData> &database)
{
    cout << "\nDatabase:\n";
    for (auto &[_, data] : database) {
        cout << "File '" << data.filename << "' has:\n";
        cout << data.swarm.size() << " clients\n";
        cout << data.segments.size() << " segments\n";
        for (int i = 0; i < data.swarm.size(); i++) {
            cout << "Client " << data.swarm[i] << " is a " << data.client_types[i] << '\n';
        }
        cout << '\n';
    }
}
