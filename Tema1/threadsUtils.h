#pragma once

#include <iostream>
#include <fstream>

#include <string>
#include <cmath>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stack>
#include <vector>
#include <unordered_map>
#include <unordered_set>

struct utils_t {
    int id;

    int mapperThreads;
    int reducerThreads;

    pthread_barrier_t barrier;
    pthread_t *threads;

    std::stack<std::string> inputFiles;
    std::vector<std::unordered_map<int, std::unordered_set<int>>> exponents;
};

void init(int argc, char *argv[], utils_t &utils);

int minim(int a, int b);

void *mapper_function(void *arg);
void *reducer_function(void *arg);
void threadCreate(utils_t &utils);

void threadJoin(utils_t &utils);

void threadsPrint();

void threadExit(utils_t &utils);