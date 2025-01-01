// Copyright (c) 2024 Horia-Valentin MOROIANU

#include "tracker.hpp"
#include "client.hpp"

void Tracker(int numtasks)
{
    unordered_map<string, TrackerData> database;
    RecvClientFiles(numtasks, database);
    int remaining_clients = numtasks - 1;

    // TODO: remove
    // PrintDatabase(database);

    bool ack = true;
    MPI_Bcast(&ack, 1, MPI_C_BOOL, TRACKER_RANK, MPI_COMM_WORLD);

    MPI_Status status;
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
            MPI_Recv(NULL, 0, MPI_CHAR, status.MPI_SOURCE, TAG_ALL_COMPLETE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            remaining_clients--;
            break;
        default:
            break;
        }
    }

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
        MPI_Recv(recv_files.data(), MAX_FILES, MPI_FILE_DATA, i, 0, MPI_COMM_WORLD, &status);

        int file_count;
        MPI_Get_count(&status, MPI_FILE_DATA, &file_count);
        recv_files.resize(file_count);

        // Save files in database
        for (auto &file : recv_files) {
            string filename(file.name);
            TrackerData &data = database[filename];
            data.swarm.push_back(i);
            data.client_types.push_back(ClientType::SEED);

            if (data.swarm.size() == 1) {
                data.file = file;
            }
        }
    }
}

// TODO: check if file exists for all these functions
void SendFile(unordered_map<string, TrackerData> &database, int source)
{
    string filename(MAX_FILENAME, '\0');
    MPI_Recv(filename.data(), MAX_FILENAME, MPI_CHAR, source, TAG_FILE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    filename.resize(strlen(filename.data()));

    TrackerData &tracker_data = database[filename];

    cerr << "Sending file " << tracker_data.file.name << " to client " << source << '\n';

    MPI_Send(&tracker_data.file, 1, MPI_FILE_DATA, source, TAG_FILE, MPI_COMM_WORLD);

    // Mark client as peer for the requested file
    tracker_data.swarm.push_back(source);
    tracker_data.client_types.push_back(ClientType::PEER);
}

void SendSwarm(unordered_map<string, TrackerData> &database, int source)
{
    string filename(MAX_FILENAME, '\0');
    MPI_Recv(filename.data(), MAX_FILENAME, MPI_CHAR, source, TAG_SWARM, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    filename.resize(strlen(filename.data()));

    TrackerData &tracker_data = database[filename];
    MPI_Send(tracker_data.swarm.data(), tracker_data.swarm.size(), MPI_INT, source, TAG_SWARM, MPI_COMM_WORLD);
}

void FileComplete(unordered_map<string, TrackerData> &database, int source)
{
    string filename(MAX_FILENAME, '\0');
    MPI_Recv(filename.data(), MAX_FILENAME, MPI_CHAR, source, TAG_F_COMPLETE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    filename.resize(strlen(filename.data()));

    TrackerData &tracker_data = database[filename];
    auto it = find(tracker_data.swarm.begin(), tracker_data.swarm.end(), source);
    tracker_data.client_types[it - tracker_data.swarm.begin()] = ClientType::SEED;
}

void PrintDatabase(unordered_map<string, TrackerData> &database)
{
    cout << "\nDatabase:\n";
    for (auto &[_, data] : database) {
        cout << "File '" << data.file.name << "' has:\n";
        cout << data.swarm.size() << " clients\n";
        cout << data.file.segment_count << " segments\n";
        for (int i = 0; i < data.swarm.size(); i++) {
            cout << "Client " << data.swarm[i] << " is a " << data.client_types[i] << '\n';
        }

        cout << "Segments:\n";
        for (int i = 0; i < data.file.segment_count; i++) {
            cout << data.file.segments[i] << '\n';
        }

        cout << '\n';
    }
}
