#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define SIZE 500000
 
int main (int argc, char *argv[])
{
    int  numtasks, rank, len;
 
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks); // Total number of processes.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // The current process ID / Rank.
 
    srand(42);
    int num1[SIZE], num2[SIZE];

    int buffer_attached_size = MPI_BSEND_OVERHEAD + SIZE * sizeof(int);
    char* buffer_attached = malloc(buffer_attached_size);
    MPI_Buffer_attach(buffer_attached, buffer_attached_size);
 
    if (rank == 0) {
        for (int i = 0; i < SIZE; i++) {
            num1[i] = 100;
        }
        printf("[%d] num1[0] before: %d\n", rank, num1[0]);
        printf("[%d] num2[0] before: %d\n", rank, num2[0]);

        // MPI_Send(&num1, SIZE, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Bsend(&num1, SIZE, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&num2, SIZE, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("[%d] num1[0] after: %d\n", rank, num1[0]);
        printf("[%d] num2[0] after: %d\n", rank, num2[0]);
    } else {
        for (int i = 0; i < SIZE; i++) {
            num2[i] = 200;
        }
        printf("[%d] num1[0] before: %d\n", rank, num1[0]);
        printf("[%d] num2[0] before: %d\n", rank, num2[0]);

        // MPI_Send(&num2, SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Bsend(&num2, SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Recv(&num1, SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("[%d] num1[0] after: %d\n", rank, num1[0]);
        printf("[%d] num2[0] after: %d\n", rank, num2[0]);
    }

    MPI_Buffer_detach(&buffer_attached, &buffer_attached_size);
    free(buffer_attached);

    MPI_Finalize();
 
}