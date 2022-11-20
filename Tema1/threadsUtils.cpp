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
    // Getting the reducer object and
    // filling out the id field of our
    // needed utils array element

    // reducer_t reducer = *(reducer_t *) arg;
    // utils_t *utilsArr = reducer.utils;
    // int id = reducer.id;
    // utilsArr[id].id = id;
    // utils_t *utils = &utilsArr[id];

    utils_t *utils = (utils_t *) arg;

    while (true) {
        // Locking the mutex so that 2 threads
        // won't read at the same time from the stack
	    pthread_mutex_lock(utils->mutex);
        string buffer;

        if (!utils->inputFiles->empty()) {
            // If the stack still contains elements, then
            // read from the top the next file
            buffer = utils->inputFiles->top();
            utils->inputFiles->pop();

            // Unlock the mutex
	        pthread_mutex_unlock(utils->mutex);
        } else {
            // If the stack is empty, then unlock the
            // mutex and stop the current thread
	        pthread_mutex_unlock(utils->mutex);
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
                for (int exp = 2; exp < utils->reducerThreads + 2; exp++) {
                    utils->exponents[exp].insert(number);
                }
            } else {
                // If the number isn't 1, then search for each exponent
                // if there is a base such as base^exp = number
                for (int exp = 2; exp < utils->reducerThreads + 2; exp++) {
                    if (isPower(exp, number)) {
                        (utils->exponents)[exp].insert(number);
                    }
                }
            }
        }

        fileReader.close();
    }

    // Wait for all threads
	pthread_barrier_wait(utils->barrier);

    // Terminate the thread
   	pthread_exit(NULL);
}

void *reducer_function(void *arg) {
    reducer_t reducer = *(reducer_t *) arg;
    utils_t *utils = reducer.utils;

	pthread_barrier_wait(utils[0].barrier);
    int mapperThreads = utils[0].mapperThreads;

    int id = reducer.id;

    unordered_set<int> reducerSet;
    for(int i = 0; i < mapperThreads; i++) {
        // TODO maybe this doesn't work
        reducerSet.insert(utils[i].exponents[id].begin(), utils[i].exponents[id].end());
    }

    char outputFile[20];
    sprintf(outputFile, "out%d.txt", id);

    ofstream fileWriter(outputFile);
    fileWriter << reducerSet.size();
    fileWriter.close();

   	pthread_exit(NULL);
}

// void *f(void *arg) {
//     reducer_t reducer = *(reducer_t *) arg;
//     utils_t *utils = reducer.utils;

//     int mapperThreads = utils[0].mapperThreads;
//     int reducerThreads = utils[0].reducerThreads;

//     cout << "Entered f"<<endl;
//     cout << mapperThreads << " " << reducer.id << endl;

//     if (reducer.id < mapperThreads) {
//         cout << "Go to mapper" <<endl;
//         mapper_function(arg);
//     } else {
//         cout << "Go to reducer" <<endl;
//         reducer_function(arg);
//     }

//   	pthread_exit(NULL);
// }

void threadCreate(utils_t **utils) {
    // Extract the mapperThreads, reducerThreads and threads
    int mapperThreads = (*utils)[0].mapperThreads;
    int reducerThreads = (*utils)[0].reducerThreads;
    pthread_t *threads = (*(*utils)[0].threads);

    reducer_t *reducerArguments = (reducer_t *)malloc(reducerThreads * sizeof(reducer_t));

    for (int i = 0; i < mapperThreads; i++) {
        (*utils)[i].id = i;
        int r = pthread_create(&threads[i], NULL, mapper_function, &((*utils)[i]));

        if (r) {
	  		perror(MAPPER_THREAD_CREATE_ERROR);
	  		exit(-1);
		}
    }

    for (int i = 0; i < reducerThreads; i++) {
        reducerArguments[i].id = i + 2;
        reducerArguments[i].utils = *utils;
        // reducer_t reducer;
        // reducer.id = i + 2;
        // cout<<"Main "<<reducer.id<<endl;
        // reducer.utils = *utils;
        int r = pthread_create(&threads[i + mapperThreads], NULL, reducer_function, &reducerArguments[i]);

        if (r) {
	  		perror(REDUCER_THREAD_CREATE_ERROR);
	  		exit(-1);
		}
    }


    // for (int i = 0; i < mapperThreads + reducerThreads; i++) {
    //     int r;

    //     // Create a new reducer element 
    //     reducerArguments[i].utils = *utils;
    //     if (i < mapperThreads) {
    //         reducerArguments[i].id = i;
    //         r = pthread_create(&threads[i], NULL, mapper_function, &reducerArguments[i]);
    //     } else {
    //         reducerArguments[i].id = i + 2 - mapperThreads;
    //         r = pthread_create(&threads[i], NULL, reducer_function, &reducerArguments[i]);
    //     }

    //     if (r) {
	//   		perror(REDUCER_THREAD_CREATE_ERROR);
	//   		exit(-1);
	// 	}
    // }
}

void threadJoin(utils_t **utils) {
    int mapperThreads = (*utils)[0].mapperThreads;
    int reducerThreads = (*utils)[0].reducerThreads;
    pthread_t *threads = (*(*utils)[0].threads);

    for (int i = 0; i < mapperThreads + reducerThreads; i++) {
        void *status;
		int r = pthread_join(threads[i], &status);

        if (r) {
	  		perror(THREAD_JOIN_ERROR);
	  		exit(-1);
		}
  	}

	pthread_mutex_destroy((*utils)[0].mutex);
	pthread_barrier_destroy((*utils)[0].barrier);
}
