#include "threadsUtils.h"

using namespace std;

void init(int argc, char *argv[], utils_t **utils) {
    
}

int minim(int a, int b) {
    if (a > b) {
        return b;
    }

    return a;
}

bool isPower(int exp, int number) {
    // TODO make binary
    for (int base = 2; base <= sqrt(number); base++) {
        int power = pow(base, exp);
        if (power == number) {
            return true;
        } else {
            if (power > number || power < 0) {
                break;
            }
        }
    }

    return false;
}

void *mapper_function(void *arg) {
    utils_t *utils = (utils_t *) arg;

    while (true) {
	    pthread_mutex_lock(utils->mutex);
        string buffer;

        if (!utils->inputFiles->empty()) {
            buffer = utils->inputFiles->top();

            cout << utils->inputFiles->size() << endl;
            utils->inputFiles->pop();
            cout << utils->inputFiles->size() << endl;

            cout<<"Thread " << utils->id << " for " << buffer <<endl;
	        pthread_mutex_unlock(utils->mutex);
        } else {
	        pthread_mutex_unlock(utils->mutex);
            break;
        }

        ifstream fileReader(buffer);

        if (!fileReader.is_open()) {
            perror(FILE_ERROR);
            exit(-1);
        }

        int number;
        fileReader >> number;

        while(fileReader >> number) {
            // Check perfect number
            if (number == 1) {
                for (int exp = 2; exp < utils->reducerThreads + 2; exp++) {
                    utils->exponents[exp].insert(number);
                }
            } else {
                for (int exp = 2; exp < utils->reducerThreads + 2; exp++) {
                    if (isPower(exp, number)) {
                        (utils->exponents)[exp].insert(number);
                    }
                }
            }
        }
    }

	pthread_barrier_wait(utils->barrier);

  	pthread_exit(NULL);
}

void *reducer_function(void *arg) {

    utils_t *utils = (utils_t *) arg;
	pthread_barrier_wait(&utils[0].barrier);
    int id = utils->id;
    int mapperThreads = utils[0].mapperThreads;
    int reducerThreads = utils[0].reducerThreads;
    string inputTestFile = utils[0].inputTestFile;

    unordered_set<int> reducerSet;

    for(int i = 0; i < mapperThreads; i++) {
        reducerSet.insert(utils[i].exponents[id].begin(), utils[i].exponents[id].end());
    }

    // TODO : Nu cred ca merge
    char outputFile[20];
    sprintf(outputFile, "out%d.txt", id);

    ofstream fileWriter(outputFile);
    fileWriter << reducerSet.size();

    cout << "Gata reducer" << endl;

  	pthread_exit(NULL);
}

void threadCreate(utils_t **utils) {
    int mapperThreads = (*utils)[0].mapperThreads;
    int reducerThreads = (*utils)[0].reducerThreads;
    pthread_t *threads = (*(*utils)[0].threads);

    for (int i = 0; i < mapperThreads; i++) {
        (*utils)[i].id = i;
        int r = pthread_create(&threads[i], NULL, mapper_function, &((*utils)[i]));

        if (r) {
	  		perror(MAPPER_THREAD_CREATE_ERROR);
	  		exit(-1);
		}
    }

    for (int i = 0; i < reducerThreads; i++) {
        (*utils)[i].id = i;
        int r = pthread_create(&threads[i], NULL, reducer_function, utils);

        if (r) {
	  		perror(REDUCER_THREAD_CREATE_ERROR);
	  		exit(-1);
		}
    }
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

void threadsPrint() {
    // TODO : change (old implementation)
    //     for (int i = 0; i < array_size; i++) {
    //     cout << arr[i];
    //     if (i != array_size - 1) {
    //         cout << " ";
    //     } else {
    //         cout << "\n";
    //     }
    // }
}

void threadExit(utils_t **utils) {
}