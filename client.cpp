// Copyright (c) 2024 Horia-Valentin MOROIANU

#include "client.hpp"

void Client(int rank)
{
    unordered_map<string, FileData> owned_files;
    vector<string> wanted_filenames;

    ReadInput(rank, owned_files, wanted_filenames);
    SendFilesToTracker(owned_files);

    bool ack = false;
    do {
        MPI_Bcast(&ack, 1, MPI_C_BOOL, TRACKER_RANK, MPI_COMM_WORLD);
    } while (!ack);

    TransferFiles(owned_files, wanted_filenames, rank);
}

void TransferFiles(unordered_map<string, FileData> &owned_files,
                   vector<string> &wanted_filenames, int rank)
{
    pthread_t download_thread;
    pthread_t upload_thread;
    int r;

    pthread_mutex_t lock;
    r = pthread_mutex_init(&lock, NULL);
    DIE(r, "Eroare la initializarea mutex-ului");

    DownloadArgs download_args = { &owned_files, &wanted_filenames, &lock, rank };
    UploadArgs upload_args = { &owned_files, &lock, rank };

    r = pthread_create(&download_thread, NULL, DownloadThread, (void *) &download_args);
    DIE(r, "Eroare la crearea thread-ului de download");

    r = pthread_create(&upload_thread, NULL, UploadThread, (void *) &upload_args);
    DIE(r, "Eroare la crearea thread-ului de upload");

    r = pthread_join(download_thread, NULL);
    DIE(r, "Eroare la asteptarea thread-ului de download");

    r = pthread_join(upload_thread, NULL);
    DIE(r, "Eroare la asteptarea thread-ului de upload");

    r = pthread_mutex_destroy(&lock);
    DIE(r, "Eroare la distrugerea mutex-ului");
}

void ReadInput(int rank, unordered_map<string, FileData> &owned_files,
               vector<string> &wanted_filenames)
{
    // TODO: REMOVE WHEN USING THE CHECKER!
    // string path = "in" + to_string(rank) + ".txt";
    string path = "../checker/tests/test1/in" + to_string(rank) + ".txt";
    ifstream fin(path);
    DIE(!fin, "Eroare la deschiderea fisierului de input");

    // Read owned files data
    int file_count;
    fin >> file_count;

    string filename;
    int segment_count;
    for (int i = 0; i < file_count; i++) {
        fin >> filename;
        fin >> segment_count;

        FileData &file = owned_files[filename];
        file.segment_count = segment_count;

        memset(file.name, '\0', MAX_FILENAME);
        strncpy(file.name, filename.data(),
                min(filename.size(), (size_t)MAX_FILENAME - 1));

        string hash;
        for (int j = 0; j < segment_count; j++) {
            fin >> hash;
            memset(file.segments[j], '\0', HASH_SIZE);
            strncpy(file.segments[j], hash.data(),
                    min(hash.size(), (size_t)HASH_SIZE - 1));
        }
    }

    // Read wanted files names
    fin >> file_count;
    wanted_filenames.resize(file_count);

    for (int i = 0; i < file_count; i++) {
        fin >> wanted_filenames[i];
    }
}

void SendFilesToTracker(unordered_map<string, FileData> &owned_files)
{
    vector<FileData> client_data;
    client_data.reserve(owned_files.size());

    for (auto &[_, file] : owned_files) {
        client_data.push_back(file);
    }
    MPI_Send(client_data.data(), client_data.size(), MPI_FILE_DATA, TRACKER_RANK, 0, MPI_COMM_WORLD);
}

void *DownloadThread(void *arg)
{
    DownloadArgs *args = (DownloadArgs *) arg;

    for (auto &filename : *args->wanted_filenames) {
        FileData file = RequestFile(filename);
        vector<int> local_swarm;
        int next_src = -1;

        for (int i = 0; i < file.segment_count; i++) {
            if (i % 10 == 0) {
                UpdateSwarm(local_swarm, filename);
            }

            DownloadSegment segment;
            do {
                next_src = (next_src + 1) % local_swarm.size();
                if (local_swarm[next_src] == args->rank) {
                    continue;
                }
                segment = RequestSegment(file.name, i, local_swarm[next_src]);
            } while (strlen(segment.hash) == 0);

            pthread_mutex_lock(args->lock);

            FileData &owned_file = (*args->owned_files)[filename];
            if (strlen(owned_file.name) == 0) {
                strcpy(owned_file.name, file.name);
                owned_file.segment_count = file.segment_count;
            }
            strcpy(owned_file.segments[i], segment.hash);

            pthread_mutex_unlock(args->lock);
        }
        MPI_Send(filename.data(), filename.size(), MPI_CHAR, TRACKER_RANK, TAG_F_COMPLETE, MPI_COMM_WORLD);
        SaveFile(file, filename, args->rank);
    }
    MPI_Send(NULL, 0, MPI_CHAR, TRACKER_RANK, TAG_ALL_COMPLETE, MPI_COMM_WORLD);
    return NULL;
}

FileData RequestFile(string &filename)
{
    MPI_Send(filename.data(), filename.size(), MPI_CHAR, TRACKER_RANK, TAG_FILE, MPI_COMM_WORLD);

    FileData file;
    MPI_Recv(&file, 1, MPI_FILE_DATA, TRACKER_RANK, TAG_FILE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return file;
}

vector<int> RequestSwarm(string &filename)
{
    MPI_Send(filename.data(), filename.size(), MPI_CHAR, TRACKER_RANK, TAG_SWARM, MPI_COMM_WORLD);

    MPI_Status status;
    vector<int> swarm(MAX_CLIENTS);
    MPI_Recv(swarm.data(), MAX_CLIENTS, MPI_INT, TRACKER_RANK, TAG_SWARM, MPI_COMM_WORLD, &status);

    int swarm_size;
    MPI_Get_count(&status, MPI_INT, &swarm_size);
    swarm.resize(swarm_size);
    return swarm;
}

void UpdateSwarm(vector<int> &local_swarm, string &filename)
{
    vector<int> tracker_swarm = RequestSwarm(filename);

    for (auto &client : tracker_swarm) {
        auto it = find(local_swarm.begin(), local_swarm.end(), client);
        if (it == local_swarm.end()) {
            local_swarm.push_back(client);
        }
    }
}

DownloadSegment RequestSegment(char *filename, int id, int peer)
{
    DownloadSegment segment;
    memset(segment.filename, '\0', MAX_FILENAME);
    memset(segment.hash, '\0', HASH_SIZE);
    strcpy(segment.filename, filename);
    segment.id = id;

    MPI_Send(&segment, 1, MPI_SEGMENT, peer, TAG_SEGMENT, MPI_COMM_WORLD);
    MPI_Recv(&segment, 1, MPI_SEGMENT, peer, TAG_SEGMENT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return segment;
}

void *UploadThread(void *arg)
{
    UploadArgs *args = (UploadArgs *) arg;

    while (true) {
        MPI_Status status;
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        switch (status.MPI_TAG) {
        case TAG_SEGMENT:
            SendSegment(args, status.MPI_SOURCE);
            break;
        case TAG_CLOSE:
            MPI_Recv(NULL, 0, MPI_CHAR, status.MPI_SOURCE, TAG_CLOSE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            return NULL;
        default:
            break;
        }
    }
    return NULL;
}

void SendSegment(UploadArgs *args, int peer)
{
    DownloadSegment segment;
    MPI_Status status;
    MPI_Recv(&segment, 1, MPI_SEGMENT, peer, TAG_SEGMENT, MPI_COMM_WORLD, &status);

    unordered_map<std::string, FileData> *owned_files = args->owned_files;

    string filename(segment.filename);
    filename.resize(strlen(segment.filename));

    pthread_mutex_lock(args->lock);

    if ((*owned_files).find(filename) != (*owned_files).end()) {
        FileData &file = (*owned_files)[filename];
        if (segment.id >= 0 && segment.id < file.segment_count) {
            strcpy(segment.hash, file.segments[segment.id]);
        }
    }

    pthread_mutex_unlock(args->lock);

    MPI_Send(&segment, 1, MPI_SEGMENT, peer, TAG_SEGMENT, MPI_COMM_WORLD);
}

void SaveFile(FileData &file, string &filename, int rank)
{
    ofstream fout("client" + to_string(rank) + "_" + filename);
    DIE(!fout, "Eroare la deschiderea fisierului de output");

    for (int i = 0; i < file.segment_count; i++) {
        fout << file.segments[i] << '\n';
    }
}
