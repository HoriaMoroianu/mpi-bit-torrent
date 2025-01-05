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

#define TAG_FILE 1          // client <-> tracker file data
#define TAG_SWARM 2         // client <-> tracker swarm
#define TAG_SEGMENT 3       // client <-> client segment data
#define TAG_F_COMPLETE 4    // client  -> tracker file complete
#define TAG_ALL_COMPLETE 5  // client  -> tracker all files complete
#define TAG_CLOSE 6         // tracker -> client close connection

#define TAG_SEGMENT_REPLY 7

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

struct DownloadSegment {
    char filename[MAX_FILENAME];
    int id;
    char hash[HASH_SIZE];
};

#endif
