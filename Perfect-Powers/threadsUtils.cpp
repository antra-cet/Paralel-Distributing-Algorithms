#include "threadsUtils.h"

using namespace std;

bool isPower(int exp, int number) {
    // TODO make binary
    // Binary search for verifying if a log(number, exp)
    // is an integer

    int start = 2;
    int end = sqrt(number);

    while (start <= end) {
        int middle = start + (end - start) / 2;

        int power = pow(middle, exp);
        if (power == number) {
            return true;
        } else {
            if (power > number || power < 0) {
               end = middle - 1;
            } else {
               start = middle + 1;
            }
        }
    }

    return false;
}

void *mapper_function(void *arg) {
    // Getting the mappers_t object
    mapper_t *mappers = (mapper_t *) arg;

    while (true) {
        // Locking the mutex so that 2 threads
        // won't read at the same time from the stack
	    pthread_mutex_lock(mappers->mutex);
        string buffer;

        if (!mappers->inputFiles->empty()) {
            // If the stack still contains elements, then
            // read from the top the next file
            buffer = mappers->inputFiles->top();
            mappers->inputFiles->pop();

            // Unlock the mutex
	        pthread_mutex_unlock(mappers->mutex);
        } else {
            // If the stack is empty, then unlock the
            // mutex and stop the current thread
	        pthread_mutex_unlock(mappers->mutex);
            break;
        }

        // Read from the file
        ifstream fileReader(buffer);
        if (!fileReader.is_open()) {
            perror(FILE_ERROR);
            exit(-1);
        }

        int number, numElements;

        // Read the number of elements in the file
        fileReader >> numElements;

        for (int i = 0; i < numElements; i++) {
            // Read the elements
            fileReader >> number;

            // Check perfect number
            if (number == 1) {
                // If the number is 1, then it should be added to
                // all the exp keys in the map
                for (int exp = 2; exp < mappers->reducerThreads + 2; exp++) {
                    mappers->exponentsMap[exp].insert(number);
                }
            } else {
                // If the number isn't 1, then search for each exponent
                // if there is a base such as base^exp = number
                for (int exp = 2; exp < mappers->reducerThreads + 2; exp++) {
                    if (isPower(exp, number)) {
                        (mappers->exponentsMap)[exp].insert(number);
                    }
                }
            }
        }

        fileReader.close();
    }

    // Wait for all threads
	pthread_barrier_wait(mappers->barrier);

    // Terminate the thread
   	pthread_exit(NULL);
}

void *reducer_function(void *arg) {
    reducer_t reducer = *(reducer_t *) arg;
    mapper_t *mappers = reducer.mappers;

	pthread_barrier_wait(mappers[0].barrier);
    int mapperThreads = mappers[0].mapperThreads;

    int id = reducer.reducerId;

    unordered_set<int> reducerSet;
    for(int i = 0; i < mapperThreads; i++) {
        // TODO maybe this doesn't work
        reducerSet.insert(mappers[i].exponentsMap[id].begin(), mappers[i].exponentsMap[id].end());
    }

    char outputFile[20];
    sprintf(outputFile, "out%d.txt", id);

    ofstream fileWriter(outputFile);
    fileWriter << reducerSet.size();
    fileWriter.close();

   	pthread_exit(NULL);
}

void threadCreate(mapper_t **mappers) {
    // Extract the mapperThreads, reducerThreads and threads
    int mapperThreads = (*mappers)[0].mapperThreads;
    int reducerThreads = (*mappers)[0].reducerThreads;
    pthread_t *threads = (*(*mappers)[0].threads);

    reducer_t *reducerArguments = (reducer_t *)malloc(reducerThreads * sizeof(reducer_t));

    // Creating threads for the mappers
    for (int i = 0; i < mapperThreads; i++) {
        (*mappers)[i].mapperId = i;
        int r = pthread_create(&threads[i], NULL, mapper_function, &((*mappers)[i]));

        if (r) {
	  		perror(MAPPER_THREAD_CREATE_ERROR);
	  		exit(-1);
		}
    }

    // Creating threads for the reducers
    for (int i = 0; i < reducerThreads; i++) {
        reducerArguments[i].reducerId = i + 2;
        reducerArguments[i].mappers = *mappers;
        int r = pthread_create(&threads[i + mapperThreads], NULL, reducer_function, &reducerArguments[i]);

        if (r) {
	  		perror(REDUCER_THREAD_CREATE_ERROR);
	  		exit(-1);
		}
    }
}

void threadJoin(mapper_t **mappers) {
    int mapperThreads = (*mappers)[0].mapperThreads;
    int reducerThreads = (*mappers)[0].reducerThreads;
    pthread_t *threads = (*(*mappers)[0].threads);

    // Joining the threads
    for (int i = 0; i < mapperThreads + reducerThreads; i++) {
        void *status;
		int r = pthread_join(threads[i], &status);

        if (r) {
	  		perror(THREAD_JOIN_ERROR);
	  		exit(-1);
		}
  	}

    // Destroying the barrier and mutex
	pthread_mutex_destroy((*mappers)[0].mutex);
	pthread_barrier_destroy((*mappers)[0].barrier);
}
