// Copyright (c) 2024 Horia-Valentin MOROIANU

#include "peer.hpp"
#include "utils.hpp"

void Peer(int numtasks, int rank)
{
    vector<pair<string, vector<string>>> files; // (filename, list of segments)
    vector<string> wanted_filenames;            // filenames to download
    ReadInput(rank, files, wanted_filenames);

    pthread_t download_thread;
    pthread_t upload_thread;
    void *status;
    int r;

    r = pthread_create(&download_thread, NULL, download_thread_func, (void *) &rank);
    DIE(r, "Eroare la crearea thread-ului de download");

    r = pthread_create(&upload_thread, NULL, upload_thread_func, (void *) &rank);
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
    cout << "Reading input from " << path << '\n';

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
            // TODO: check hash size?
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

void *download_thread_func(void *arg)
{
    int rank = *(int*) arg;
    cout << "Download thread " << rank << " started\n";
    return NULL;
}

void *upload_thread_func(void *arg)
{
    int rank = *(int*) arg;
    cout << "Upload thread " << rank << " started\n";
    return NULL;
}
