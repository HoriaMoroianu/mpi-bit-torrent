// Copyright (c) 2024 Horia-Valentin MOROIANU

#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <iostream>

#define TRACKER_RANK 0
#define MAX_FILES 10
#define MAX_FILENAME 15
#define HASH_SIZE 33
#define MAX_CHUNKS 100

#define DIE(assertion, call_description)            \
    do {                                            \
        if (assertion) {                            \
            std::cerr << call_description << '\n';  \
            std::exit(EXIT_FAILURE);                \
        }                                           \
    } while (0)

#endif
