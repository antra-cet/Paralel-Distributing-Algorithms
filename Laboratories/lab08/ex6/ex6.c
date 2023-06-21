#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define GROUP_SIZE 4

int main (int argc, char *argv[])
{
    int old_size, new_size;
    int old_rank, new_rank;
    int recv_rank;
    MPI_Comm custom_group;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &old_size); // Total number of processes.
    MPI_Comm_rank(MPI_COMM_WORLD, &old_rank); // The current process ID / Rank.

    // Split the MPI_COMM_WORLD in small groups.
    int color = old_rank / GROUP_SIZE;
    int key = old_rank % GROUP_SIZE;
    MPI_Comm_split(MPI_COMM_WORLD, color, key, &custom_group);

    MPI_Comm_size(MPI_COMM_WORLD, &new_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &new_rank);

    printf("Rank [%d] / size [%d] in MPI_COMM_WORLD and rank [%d] / size [%d] in custom group.\n",
            old_rank, old_size, new_rank, new_size);

    // Send the rank to the next process.

    if (new_rank == 0) {
        // First process starts the circle.
        // Send the number to the next process.
        int send_rank = new_rank;
        MPI_Send(&send_rank, 1, MPI_INT, 1, 0, custom_group);
    } else if (new_rank == GROUP_SIZE - 1) {
        // Last process close the circle.
        // Receives the number from the previous process.
        // Sends the number to the first process.
        MPI_Recv(&recv_rank, 1, MPI_INT, new_rank - 1, 0, custom_group, MPI_STATUS_IGNORE);
        int send_rank = new_rank;
        MPI_Send(&send_rank, 1, MPI_INT, 0, 0, custom_group);
    } else {
        // Middle process.
        // Receives the number from the previous process.
        // Sends the number to the next process.
        MPI_Recv(&recv_rank, 1, MPI_INT, new_rank - 1, 0, custom_group, MPI_STATUS_IGNORE);
        int send_rank = new_rank;
        MPI_Send(&send_rank, 1, MPI_INT, new_rank + 1, 0, custom_group);
    }

    // Receive the rank.
    printf("Process [%d] from group [%d] received [%d].\n", new_rank,
            old_rank / GROUP_SIZE, recv_rank);

    MPI_Finalize();
}
