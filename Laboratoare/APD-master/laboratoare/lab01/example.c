#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 2

void *f(void *arg) {
  	long id = *(long*)arg;
	// for (int i = 0; i < 100; i++) {
  	// 	printf("Hello World din thread-ul %ld!\n", id);
	// }

  	printf("Hello World din thread-ul %ld!\n", id);
  	pthread_exit(NULL);
}

void *g(void *arg) {
  	long id = *(long*)arg;

  	printf("Buna Gigi %ld!\n", id);
  	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	// long cores = sysconf(_SC_NPROCESSORS_CONF);

	pthread_t threads[NUM_THREADS];
  	int r;
  	long id;
  	void *status;
	long ids[NUM_THREADS];

  	for (id = 0; id < NUM_THREADS; id++) {
		ids[id] = id; 
		r = pthread_create(&threads[id], NULL, f, &ids[id]);

		if (r) {
	  		printf("Eroare la crearea thread-ului %ld\n", id);
	  		exit(-1);
		}
  	}

	if (id == 0) {
		ids[id] = id; 
		r = pthread_create(&threads[id], NULL, f, &ids[id]);

		if (r) {
	  		printf("Eroare la crearea thread-ului %ld\n", id);
	  		exit(-1);
		}
	} else {
		ids[id] = id; 
		r = pthread_create(&threads[id], NULL, g, &ids[id]);

		if (r) {
	  		printf("Eroare la crearea thread-ului %ld\n", id);
	  		exit(-1);
		}
	}

  	for (id = 0; id < NUM_THREADS; id++) {
		r = pthread_join(threads[id], &status);

		if (r) {
	  		printf("Eroare la asteptarea thread-ului %ld\n", id);
	  		exit(-1);
		}
  	}

  	pthread_exit(NULL);
}
