// Copyright (c) 2024 Horia-Valentin MOROIANU

#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <iostream>

#define TRACKER_RANK 0
#define MAX_FILES 10
#define MAX_FILENAME 15
#define HASH_SIZE 33
#define MAX_CHUNKS 100
#define MAX_CLIENTS 20

#define TAG_F_REQUEST 1
#define TAG_F_REPLY 2
#define TAG_SWARM 3
#define TAG_F_COMPLETE 4
#define TAG_ALL_COMPLETE 5

#define DIE(assertion, call_description)            \
    do {                                            \
        if (assertion) {                            \
            std::cerr << call_description << '\n';  \
            std::exit(EXIT_FAILURE);                \
        }                                           \
    } while (0)

struct FileData {
    char name[MAX_FILENAME];
    int segment_count;
    char segments[MAX_CHUNKS][HASH_SIZE];
};

struct SwarmData {
    FileData file;
    int swarm_size;
    int swarm[MAX_CLIENTS];
};

#endif
