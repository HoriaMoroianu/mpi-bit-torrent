// Copyright (c) 2024 Horia-Valentin MOROIANU

#include "tracker.hpp"
#include "client.hpp"

void Tracker(int numtasks, int rank)
{
    cout << "Tracker started\n";
    unordered_map<string, TrackerData> database;
    RecvClientFiles(numtasks, database);
    // TODO: remove
    PrintDatabase(database);

    bool ack = true;
    MPI_Bcast(&ack, 1, MPI_C_BOOL, TRACKER_RANK, MPI_COMM_WORLD);

    MPI_Status status;
    while (true) {
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        switch (status.MPI_TAG) {
        case TAG_F_REQUEST:
            FileRequest(database, status.MPI_SOURCE);
            break;
        case TAG_F_COMPLETE:
            break;
        case TAG_ALL_COMPLETE:
            break;
        default:
            break;
        }
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
            TrackerData &data = database[file.name];
            data.swarm.push_back(i);
            data.client_types.push_back(ClientType::SEED);

            if (data.swarm.size() == 1) {
                data.file = file;
            }
        }
    }
}

void FileRequest(unordered_map<string, TrackerData> &database, int source)
{
    // string filename(MAX_FILENAME, '\0');
    // MPI_Recv(filename.data(), MAX_FILENAME, MPI_CHAR, source, TAG_F_REQUEST, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // TrackerData &tracker_data = database[filename];
    // SwarmData swarm_data;
    // swarm_data.file = tracker_data.file;
    // swarm_data.swarm_size = tracker_data.swarm.size();
    // for (int i = 0; i < tracker_data.swarm.size(); i++) {
    //     swarm_data.swarm[i] = tracker_data.swarm[i];
    // }

    // MPI_Send(&swarm_data, 1, MPI_SWARM_DATA, source, TAG_F_REPLY, MPI_COMM_WORLD);

    // // Mark client as peer for the requested file
    // tracker_data.swarm.push_back(source);
    // tracker_data.client_types.push_back(ClientType::PEER);
}

void FileComplete(unordered_map<string, TrackerData> &database, int source)
{
    string filename(MAX_FILENAME, '\0');
    MPI_Recv(filename.data(), MAX_FILENAME, MPI_CHAR, source, TAG_F_COMPLETE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    TrackerData &tracker_data = database[filename];
    for (int i = 0; i < tracker_data.swarm.size(); i++) {
        if (tracker_data.swarm[i] == source) {
            tracker_data.client_types[i] = ClientType::SEED;
            break;
        }
    }
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
