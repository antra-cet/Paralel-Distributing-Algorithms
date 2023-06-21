#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
    int numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int recv_num;
    MPI_Request request;
    MPI_Status status;
    int flag;

    // First process starts the circle.
    if (rank == 0) {
        // First process starts the circle.
        // Generate a random number.
        // Send the number to the next process.
        int rand_val = rand() % 100;
        printf("The initial value is: %d.\n", rand_val);
        
        MPI_Isend(&rand_val, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &request);
        MPI_Test(&request, &flag, &status);
        if (flag) {
            printf("[P%d] The send operation is over.\n", rank);
        } else {
            printf("[P%d] The send operation is not over yet.\n", rank);
            MPI_Wait(&request, &status);
        }
        printf("[P%d] The send operation is definitely over.\n", rank);

        MPI_Irecv(&recv_num, 1, MPI_INT, numtasks - 1, 0, MPI_COMM_WORLD,
                    &request);
        MPI_Test(&request, &flag, &status);
        if (flag) {
            printf("[P%d] The receive operation is over.\n", rank);
        } else {
            printf("[P%d] The receive operation is not over yet.\n", rank);
            MPI_Wait(&request, &status);
        }
        printf("Process %d received %d.\n", rank, recv_num);

    } else if (rank == numtasks - 1) {
        // Last process close the circle.
        // Receives the number from the previous process.
        // Increments the number.
        // Sends the number to the first process.
        MPI_Irecv(&recv_num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD,
                    &request);
        MPI_Test(&request, &flag, &status);
        if (flag) {
            printf("[P%d] The receive operation is over.\n", rank);
        } else {
            printf("[P%d] The receive operation is not over yet.\n", rank);
            MPI_Wait(&request, &status);
        }
        printf("Process %d received %d.\n", rank, recv_num);

        recv_num += 2;

        MPI_Isend(&recv_num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
        MPI_Test(&request, &flag, &status);
        if (flag) {
            printf("[P%d] The send operation is over.\n", rank);
        } else {
            printf("[P%d] The send operation is not over yet.\n", rank);
            MPI_Wait(&request, &status);
        }
        printf("[P%d] The send operation is definitely over.\n", rank);

    } else {
        // Middle process.
        // Receives the number from the previous process.
        // Increments the number.
        // Sends the number to the next process.
        MPI_Irecv(&recv_num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD,
                    &request);
        MPI_Test(&request, &flag, &status);
        if (flag) {
            printf("[P%d] The receive operation is over.\n", rank);
        } else {
            printf("[P%d] The receive operation is not over yet.\n", rank);
            MPI_Wait(&request, &status);
        }
        printf("Process %d received %d.\n", rank, recv_num);

        recv_num += 2;

        MPI_Isend(&recv_num, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &request);
        MPI_Test(&request, &flag, &status);
        if (flag) {
            printf("[P%d] The send operation is over.\n", rank);
        } else {
            printf("[P%d] The send operation is not over yet.\n", rank);
            MPI_Wait(&request, &status);
        }
        printf("[P%d] The send operation is definitely over.\n", rank);
    }

    MPI_Finalize();

}
