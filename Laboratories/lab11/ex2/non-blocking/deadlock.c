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

    MPI_Request requests[2];
    MPI_Status status;
    int flag;
 
    if (rank == 0) {
        for (int i = 0; i < SIZE; i++) {
            num1[i] = 100;
        }

        MPI_Isend(&num1, SIZE, MPI_INT, 1, 0, MPI_COMM_WORLD, &requests[0]);
        MPI_Irecv(&num2, SIZE, MPI_INT, 1, 0, MPI_COMM_WORLD, &requests[1]);
    } else {
        for (int i = 0; i < SIZE; i++) {
            num2[i] = 200;
        }

        MPI_Isend(&num2, SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, &requests[0]);
        MPI_Irecv(&num1, SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, &requests[1]);
    }

    MPI_Waitall(2, requests, MPI_STATUSES_IGNORE);
    printf("Process %d: done. num1[0] = %d. num2[0] = %d\n", rank, num1[0],
            num2[0]);

    MPI_Finalize();
 
}