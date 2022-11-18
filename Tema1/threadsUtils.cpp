#include "threadsUtils.h"

using namespace std;

#define INIT_ERROR "Unable to process the request.\nThe format must be : ./threads [number_mapper_threads] [number_reducer_threads] [input_file]\n"
#define MAPPER_THREAD_CREATE_ERROR "Unable to create the thread for the mapper.\n"
#define REDUCER_THREAD_CREATE_ERROR "Unable to create the thread for the reducer.\n"
#define THREAD_JOIN_ERROR "Error while waiting to join the thread.\n"

#define MAX_OFFSET 5
#define MAX_FILENAME 100
#define MAX_ARR_SIZE 100


void init(int argc, char *argv[], utils_t &utils) {
    if (argc < 4) {
        perror(INIT_ERROR);
        exit(-1);
    }

    utils.mapperThreads = atoi(argv[1]);
    utils.reducerThreads = atoi(argv[2]);
    string inputFile = argv[3];

    utils.threads = (pthread_t *) calloc((utils.mapperThreads + utils.reducerThreads + MAX_OFFSET),
                    sizeof(pthread_t));
    for (int i = 0; i < utils.reducerThreads; i++) {
        std::unordered_map<int, std::unordered_set<int>> myMap;
        utils.exponents.push_back(myMap);
    }
    pthread_barrier_init(&utils.barrier, NULL, utils.mapperThreads);

    // Reading the files from the test.txt files
    int numberOfFiles = 0;
    string buffer;
    ifstream fileReader(inputFile);

    fileReader >> numberOfFiles;
    getline(fileReader, buffer);
    for (int i = 0; i < numberOfFiles; i++) {
        getline(fileReader, buffer);
        utils.inputFiles.push(buffer);
    }
}

int minim(int a, int b) {
    if (a > b) {
        return b;
    }

    return a;
}

bool isPower(int x, long int y) {
    long int pow = 1;
    while (pow < y) {
        pow *= x;
    }
 
    return (pow == y);
}

void *mapper_function(void *arg) {
    utils_t utils = *(utils_t *) arg;

    while (!utils.inputFiles.empty()) {
        string buffer = utils.inputFiles.top();
        utils.inputFiles.pop();
        ifstream fileReader(buffer);

        while(getline(fileReader, buffer)) {
            int number = stoi(buffer);
            cout << number << " ";

            // Check perfect number
            for (int exp = 2; exp <= utils.reducerThreads; exp++) {
                if (isPower(exp, number)) {
                    utils.exponents.at(utils.id)[exp].insert(number);
                }
            }
        }
    }
	pthread_barrier_wait(&utils.barrier);

  	pthread_exit(NULL);
}

void *reducer_function(void *arg) {
    utils_t utils = *(utils_t *) arg;
	pthread_barrier_wait(&utils.barrier);

    cout << "Saleu";

  	pthread_exit(NULL);
}

void threadCreate(utils_t &utils) {
    for (int i = 0; i < utils.mapperThreads; i++) {
        utils.id = i;
        int r = pthread_create(&utils.threads[i], NULL, mapper_function, &utils);

        if (r) {
	  		perror(MAPPER_THREAD_CREATE_ERROR);
	  		exit(-1);
		}
    }

    for (int i = 0; i < utils.reducerThreads; i++) {
        utils.id = i;
        int r = pthread_create(&utils.threads[i], NULL, reducer_function, &utils);

        if (r) {
	  		perror(REDUCER_THREAD_CREATE_ERROR);
	  		exit(-1);
		}
    }
}

void threadJoin(utils_t &utils) {
    // TODO : change (old implementation)
    for (int i = 0; i < utils.mapperThreads + utils.reducerThreads + 1; i++) {
        void *status;
		int r = pthread_join(utils.threads[i], &status);

        if (r) {
	  		perror(THREAD_JOIN_ERROR);
	  		exit(-1);
		}
  	}
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

void threadExit(utils_t &utils) {
	pthread_barrier_destroy(&utils.barrier);
  	pthread_exit(NULL);
}