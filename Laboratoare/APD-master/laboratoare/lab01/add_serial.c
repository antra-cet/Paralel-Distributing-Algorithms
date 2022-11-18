#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/*
    schelet pentru exercitiul 5
*/

int* arr;
int array_size;
#define P 6


int minim(int a, int b) {
    if (a > b) {
        return b;
    }

    return a;
}

void *f(void *arg) {
  	int id = *(int*)arg;
    int start = id * (double)array_size / P;
    int end = minim((id + 1) * (double)array_size / P, array_size);

    for (int i = start; i < end; i++) {
        arr[i] += 100;
    }

  	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  	void *status;

    if (argc < 2) {
        perror("Specificati dimensiunea array-ului\n");
        exit(-1);
    }

    array_size = atoi(argv[1]);

    arr = malloc(array_size * sizeof(int));

    for (int i = 0; i < array_size; i++) {
        arr[i] = i;
    }

    for (int i = 0; i < array_size; i++) {
        printf("%d", arr[i]);
        if (i != array_size - 1) {
            printf(" ");
        } else {
            printf("\n");
        }
    }

    int r;
    pthread_t threads[array_size];
    int i;

    // TODO: aceasta operatie va fi paralelizata
  	for (i = 0; i < P; i++) {
        r = pthread_create(&threads[i], NULL, f, &i);

		if (r) {
	  		printf("Eroare la crearea thread-ului %d\n", i);
	  		exit(-1);
		}
    }

    for (i = 0; i < P; i++) {
		r = pthread_join(threads[i], &status);

		if (r) {
	  		printf("Eroare la asteptarea thread-ului %d\n", i);
	  		exit(-1);
		}
  	}

    for (int i = 0; i < array_size; i++) {
        printf("%d", arr[i]);
        if (i != array_size - 1) {
            printf(" ");
        } else {
            printf("\n");
        }
    }

  	pthread_exit(NULL);
}
