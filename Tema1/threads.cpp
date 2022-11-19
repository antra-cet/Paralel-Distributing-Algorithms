#include <iostream>
#include "threadsUtils.h"

using namespace std;

int main(int argc, char *argv[]) {
    utils_t *utils;

    if (argc < 4) {
        perror(INIT_ERROR);
        exit(-1);
    }

    int mapperThreads = atoi(argv[1]);
    int reducerThreads = atoi(argv[2]);
    string inputTestFile = argv[3];

    // Initializing the barriers and mutexes
    pthread_barrier_t barrier;
    pthread_mutex_t mutex;

    pthread_barrier_init(&barrier, NULL, mapperThreads + reducerThreads);
	pthread_mutex_init(&mutex, NULL);

    // Initializing the threads
    pthread_t *threads = (pthread_t *) calloc((mapperThreads + reducerThreads),
                        sizeof(pthread_t));

    
    // Reading the files from the test.txt files
    int numberOfFiles = 0;
    string buffer;
    ifstream fileReader(inputTestFile);

    if (!fileReader.is_open()) {
        perror(FILE_ERROR);
        exit(-1);
    }

    std::stack<std::string> inputFiles;

    fileReader >> numberOfFiles;
    for (int i = 0; i < numberOfFiles; i++) {
        fileReader>>buffer;
        inputFiles.push(buffer);
    }

    utils = (utils_t *) calloc((mapperThreads + reducerThreads + 1), sizeof(utils_t));
    for (int i = 0; i < mapperThreads + reducerThreads; i++) {
        utils[i].mapperThreads = mapperThreads;
        utils[i].reducerThreads = reducerThreads;
        utils[i].inputTestFile = inputTestFile;

        utils[i].threads = &threads;
        utils[i].barrier = &barrier;
        utils[i].mutex = &mutex;

        utils[i].inputFiles = &inputFiles;
        utils[i].exponents.reserve(reducerThreads + 2);
    }
    // init(argc, argv, &utils);

    threadCreate(&utils);
    threadJoin(&utils);
    // threadsPrint();
    // threadExit(&utils);

  	pthread_exit(NULL);
}
