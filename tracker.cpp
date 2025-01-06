// Copyright (c) 2024 Horia-Valentin MOROIANU

#include "tracker.hpp"
#include "client.hpp"

void Tracker(int numtasks)
{
    unordered_map<string, TrackerData> database;
    RecvClientFiles(numtasks, database);

    // Signal clients to start the transfer
    MPI_Bcast(NULL, 0, MPI_CHAR, TRACKER_RANK, MPI_COMM_WORLD);

    MPI_Status status;
    int remaining_clients = numtasks - 1;
    while (remaining_clients > 0) {
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        switch (status.MPI_TAG) {
        case TAG_FILE:
            SendFile(database, status.MPI_SOURCE);
            break;
        case TAG_SWARM:
            SendSwarm(database, status.MPI_SOURCE);
            break;
        case TAG_F_COMPLETE:
            FileComplete(database, status.MPI_SOURCE);
            break;
        case TAG_ALL_COMPLETE:
            MPI_Recv(NULL, 0, MPI_CHAR, status.MPI_SOURCE,
                     TAG_ALL_COMPLETE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            remaining_clients--;
            break;
        default:
            break;
        }
    }

    // Signal clients to close
    for (int i = 1; i < numtasks; i++) {
        MPI_Send(NULL, 0, MPI_CHAR, i, TAG_CLOSE, MPI_COMM_WORLD);
    }
}

void RecvClientFiles(int numtasks, unordered_map<string, TrackerData> &database)
{
    MPI_Status status;

    for (int i = 1; i < numtasks; i++) {
        // Receive files from client i
        vector<FileData> recv_files(MAX_FILES);
        MPI_Recv(recv_files.data(), MAX_FILES, MPI_FILE_DATA, i,
                 0, MPI_COMM_WORLD, &status);

        int file_count;
        MPI_Get_count(&status, MPI_FILE_DATA, &file_count);
        recv_files.resize(file_count);

        // Save files in database
        for (auto &file : recv_files) {
            string filename(file.name);
            TrackerData &data = database[filename];
            data.swarm.push_back(i);
            data.client_types.push_back(TrackerData::SEED);

            if (data.swarm.size() == 1) {
                data.file = file;
            }
        }
    }
}

string RecvFilename(int source, int tag)
{
    string filename(MAX_FILENAME, '\0');
    MPI_Recv(filename.data(), MAX_FILENAME, MPI_CHAR, source,
             tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    filename.resize(strlen(filename.data()));
    return filename;
}

void SendFile(unordered_map<string, TrackerData> &database, int source)
{
    string filename = RecvFilename(source, TAG_FILE);

    // Send file to client
    TrackerData &tracker_data = database[filename];
    MPI_Send(&tracker_data.file, 1, MPI_FILE_DATA, source,
             TAG_FILE, MPI_COMM_WORLD);

    // Mark client as peer for the requested file
    tracker_data.swarm.push_back(source);
    tracker_data.client_types.push_back(TrackerData::PEER);
}

void SendSwarm(unordered_map<string, TrackerData> &database, int source)
{
    string filename = RecvFilename(source, TAG_SWARM);
    // Send swarm to client
    vector<int> &swarm = database[filename].swarm;
    MPI_Send(swarm.data(), swarm.size(), MPI_INT, source,
             TAG_SWARM, MPI_COMM_WORLD);
}

void FileComplete(unordered_map<string, TrackerData> &database, int source)
{
    string filename = RecvFilename(source, TAG_F_COMPLETE);

    // Mark client as seed for the completed file
    TrackerData &tracker_data = database[filename];
    auto it = find(tracker_data.swarm.begin(),
                   tracker_data.swarm.end(),
                   source);

    int index = it - tracker_data.swarm.begin();
    tracker_data.client_types[index] = TrackerData::SEED;
}
