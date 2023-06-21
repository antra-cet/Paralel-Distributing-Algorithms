#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>

typedef struct {
    int size;
    int arr[1000];
} queue;

#define FIELDS 2

int main (int argc, char *argv[]) {
    int numtasks, rank;

    queue q;
    // declare the types and arrays for offsets and block counts
    MPI_Datatype customtype, oldtypes[FIELDS];
    int blockcounts[FIELDS];
    MPI_Aint offsets[FIELDS];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // create the MPI data type, using offsets and block counts, and
    // commit the data type

    // int size
    offsets[0] = offsetof(queue, size);
    oldtypes[0] = MPI_INT;
    blockcounts[0] = 1;

    // int arr[1000]
    offsets[1] = offsetof(queue, arr);
    oldtypes[1] = MPI_INT;
    blockcounts[1] = 1000;

    MPI_Type_create_struct(FIELDS, blockcounts, offsets, oldtypes, &customtype);
    MPI_Type_commit(&customtype);

    srand(time(NULL) + rank);
 
    // First process starts the circle.
    if (rank == 0) {
        // First process starts the circle.
        // Generate a random number and add it in queue.
        // Sends the queue to the next process.
        q.size = 0;
        int rand_val = rand() % 100;
        printf("[%d] Random number: %d\n", rank, rand_val);
        q.arr[q.size++] = rand_val;

        MPI_Send(&q, 1, customtype, rank + 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&q, 1, customtype, numtasks - 1, 0, MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE);

    } else if (rank == numtasks - 1) {
        // Last process close the circle.
        // Receives the queue from the previous process.
        // Generate a random number and add it in queue.
        // Sends the queue to the first process.
        MPI_Recv(&q, 1, customtype, rank - 1, 0, MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE);

        int rand_val = rand() % 100;
        printf("[%d] Random number: %d\n", rank, rand_val);
        q.arr[q.size++] = rand_val;

        MPI_Send(&q, 1, customtype, 0, 0, MPI_COMM_WORLD);
    } else {
        // Middle process.
        // Receives the queue from the previous process.
        // Generate a random number and add it in queue.
        // Sends the queue to the next process.
        MPI_Recv(&q, 1, customtype, rank - 1, 0, MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE);

        int rand_val = rand() % 100;
        printf("[%d] Random number: %d\n", rank, rand_val);
        q.arr[q.size++] = rand_val;

        MPI_Send(&q, 1, customtype, rank + 1, 0, MPI_COMM_WORLD);
    }

    // Process 0 prints the elements from queue
    if (rank == 0) {
        printf("Final queue: ");
        for (int i = 0; i < q.size; i++) {
            printf("%d ", q.arr[i]);
        }
        printf("\n");
    }
    
    // free the newly created MPI data type
    MPI_Type_free(&customtype);

    MPI_Finalize();
}