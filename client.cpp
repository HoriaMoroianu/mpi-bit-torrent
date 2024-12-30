// Copyright (c) 2024 Horia-Valentin MOROIANU

#include "client.hpp"

void Client(int numtasks, int rank)
{
    // List of pairs (filename, list of segments)
    vector<pair<string, vector<string>>> owned_files;
    vector<string> wanted_filenames;    // filenames to download

    ReadInput(rank, owned_files, wanted_filenames);
    SendFilesToTracker(owned_files);

    bool ack = false;
    do {
        MPI_Bcast(&ack, 1, MPI_C_BOOL, TRACKER_RANK, MPI_COMM_WORLD);
    } while (!ack);

    pthread_t download_thread;
    pthread_t upload_thread;
    void *status;
    int r;

    r = pthread_create(&download_thread, NULL, DownloadThread, (void *) &rank);
    DIE(r, "Eroare la crearea thread-ului de download");

    r = pthread_create(&upload_thread, NULL, UploadThread, (void *) &rank);
    DIE(r, "Eroare la crearea thread-ului de upload");

    r = pthread_join(download_thread, &status);
    DIE(r, "Eroare la asteptarea thread-ului de download");

    r = pthread_join(upload_thread, &status);
    DIE(r, "Eroare la asteptarea thread-ului de upload");
}

void ReadInput(int rank, vector<pair<string, vector<string>>> &files,
               vector<string> &wanted_filenames)
{
    // TODO: REMOVE WHEN USING THE CHECKER!
    string path = "../checker/tests/test1/in" + to_string(rank) + ".txt";
    ifstream fin(path);
    DIE(!fin, "Eroare la deschiderea fisierului de input");

    // Read owned files data
    int owned_files;
    fin >> owned_files;
    files.resize(owned_files);

    string filename;
    int segment_count;
    for (int i = 0; i < owned_files; i++) {
        fin >> files[i].first;  // filename
        fin >> segment_count;

        vector<string> &segments = files[i].second;
        segments.resize(segment_count);

        for (int j = 0; j < segment_count; j++) {
            fin >> segments[j];
        }
    }

    // Read wanted files names
    int wanted_files;
    fin >> wanted_files;
    wanted_filenames.resize(wanted_files);

    for (int i = 0; i < wanted_files; i++) {
        fin >> wanted_filenames[i];
    }
}

void SendFilesToTracker(vector<pair<string, vector<string>>> &files)
{
    vector<FileData> client_data;
    client_data.reserve(files.size());

    for (auto &[filename, segments] : files) {
        FileData file_data;
        strncpy(file_data.filename, filename.data(), filename.size());
        file_data.filename[filename.size()] = '\0';

        for (int i = 0; i < segments.size(); i++) {
            strncpy(file_data.segments[i], segments[i].data(), segments[i].size());
            file_data.segments[i][segments[i].size()] = '\0';
        }
        file_data.segment_count = segments.size();
        client_data.push_back(file_data);
    }

    MPI_Send(client_data.data(), client_data.size(), MPI_FILE_DATA, TRACKER_RANK, 0, MPI_COMM_WORLD);
}

void *DownloadThread(void *arg)
{
    int rank = *(int*) arg;
    
    // TODO: recv from args
    // TODO: mutex + update realtime owned files

    vector<string> wanted_filenames;

    for (auto &filename : wanted_filenames) {
        MPI_Send(filename.data(), filename.size(), MPI_CHAR, TRACKER_RANK, TAG_F_REQUEST, MPI_COMM_WORLD);

        SwarmData swarm_data;
        MPI_Recv(&swarm_data, 1, MPI_SWARM_DATA, TRACKER_RANK, TAG_F_REPLY, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // For round robin
        // TODO: compare function
        priority_queue<pair<int, int>> swarm;
        unordered_set<int> clients;
        for (int i = 0; i < swarm_data.swarm_size; i++) {
            swarm.push({ swarm_data.swarm[i], 0 });
            clients.insert(swarm_data.swarm[i]);
        }

        for (int i = 0; i < swarm_data.file.segment_count; i++) {
            auto &[source, req_count] = swarm.top();
            swarm.pop();

            // MPI_send(...);
            // MPI_recv(...);

            // if (ok) {
            //     req_count++;
            //     swarm.push({ source, req_count });
            // } else {
            //     swarm.push({ source, req_count });
            //     retry;
            // }
        }
    }

    return NULL;
}

void *UploadThread(void *arg)
{
    int rank = *(int*) arg;
    // cout << "Upload thread " << rank << " started\n";
    return NULL;
}
