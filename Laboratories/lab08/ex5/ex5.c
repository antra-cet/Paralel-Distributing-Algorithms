#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROOT 0

int main (int argc, char *argv[])
{
    int  numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    // Checks the number of processes allowed.
    if (numtasks != 2) {
        printf("Wrong number of processes. Only 2 allowed!\n");
        MPI_Finalize();
        return 0;
    }

    // How many numbers will be sent.
    int send_numbers = 10;

    srand(time(NULL));

    if (rank == 0) {
        for (int i = 0; i < send_numbers; i++) {
            // Generate the random numbers.
            // Generate the random tags.
            // Sends the numbers with the tags to the second process.
            int rand_num = rand() % 100 + 50;
            int rand_tag = rand() % 10;
            MPI_Send(&rand_num, 1, MPI_INT, 1, rand_tag, MPI_COMM_WORLD);
        }
    } else {
        int recv = 0, value;

        while (recv < send_numbers) {
            // Receives the information from the first process.
            // Prints the numbers with their corresponding tags.
            MPI_Status status;
            MPI_Recv(&value, 1, MPI_INT, ROOT, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            printf("Received number %d with tag %d.\n", value, status.MPI_TAG);
            recv++;
        }
    }

    MPI_Finalize();

}
