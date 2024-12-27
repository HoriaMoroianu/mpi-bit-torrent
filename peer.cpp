// Copyright (c) 2024 Horia-Valentin MOROIANU

#include "peer.hpp"
#include "utils.hpp"

void peer(int numtasks, int rank)
{
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
