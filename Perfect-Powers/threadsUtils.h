#pragma once

#include <iostream>
#include <fstream>

#include <string>
#include <cmath>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stack>
#include <vector>
#include <unordered_map>
#include <unordered_set>


#define INIT_ERROR "Unable to process the request.\nThe format must be : ./threads [number_mapper_threads] [number_reducer_threads] [input_file]\n"
#define MAPPER_THREAD_CREATE_ERROR "Unable to create the thread for the mapper.\n"
#define REDUCER_THREAD_CREATE_ERROR "Unable to create the thread for the reducer.\n"
#define THREAD_JOIN_ERROR "Error while waiting to join the thread.\n"
#define FILE_ERROR "Coldn't open the file to read.\n"

// The mapper structures that remembers the necessary fields
struct mapper_t {
    int mapperThreads;
    int reducerThreads;

    pthread_barrier_t *barrier;
    pthread_mutex_t *mutex;
    pthread_t **threads;

    std::stack<std::string> *inputFiles;

    int mapperId;
    std::unordered_map<int, std::unordered_set<int>> exponentsMap;
};

// The reducer structure used to remember utils and the additional
// id for the reducers
struct reducer_t {
    int reducerId;
    mapper_t *mappers;
};

void *mapper_function(void *arg);
void *reducer_function(void *arg);

void threadCreate(mapper_t **mappers);
void threadJoin(mapper_t **mappers);