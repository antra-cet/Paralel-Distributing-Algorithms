#include <iostream>
#include "threadsUtils.h"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 4) {
        perror(INIT_ERROR);
        exit(-1);
    }

    // Filling out the needed fields
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
    std::stack<std::string> inputFiles;

    ifstream fileReader(inputTestFile);
    if (!fileReader.is_open()) {
        perror(FILE_ERROR);
        exit(-1);
    }

    fileReader >> numberOfFiles;
    for (int i = 0; i < numberOfFiles; i++) {
        // Placing all the input files into a stack
        fileReader>>buffer;
        inputFiles.push(buffer);
    }

    fileReader.close();

    // Declaring and filling the parameters of my mappers structure
    mapper_t *mappers;
    mappers = (mapper_t *) calloc((mapperThreads + reducerThreads + 1), sizeof(mapper_t));
    for (int i = 0; i < mapperThreads + reducerThreads; i++) {
        mappers[i].mapperThreads = mapperThreads;
        mappers[i].reducerThreads = reducerThreads;

        mappers[i].threads = &threads;
        mappers[i].barrier = &barrier;
        mappers[i].mutex = &mutex;

        mappers[i].inputFiles = &inputFiles;
        mappers[i].exponentsMap.reserve(reducerThreads + 2);
    }

    // Creating the threads for the mappers/ reducers
    threadCreate(&mappers);

    // Joining all the threads
    threadJoin(&mappers);

    // Exiting the threads
  	pthread_exit(NULL);
}
