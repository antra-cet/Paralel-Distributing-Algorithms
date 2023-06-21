#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MAX_LEN_FILENAME 100
#define MAX_LEN_PATH 50
#define NUM_CLUSTERS 4

#define CHECKER_CL "cluster"
#define TEST1_CL "../checker/tests/test1/cluster"
#define TEST2_CL "../checker/tests/test2/cluster"
#define TEST3_CL "../checker/tests/test3/cluster"

#define TXT ".txt"

int size, dim_vect;

void print_clusters(int rank, int *procs, int num_procs_controlled[4]);
void send_to_workers(int rank, int *procs, int *num_procs_controlled, int *v);
void receive_from_workers(int rank, int *procs, int num_procs_controlled[4], int *v);
void receive_for_workers(int rank);

int min(int a, int b) {
    return (a > b) ? b : a;
}

int main(int argc, char *argv[])
{
    // Defining the file to open - for manual TESTX_CL,
    // for checker CHECKER_CL
    char filename[MAX_LEN_FILENAME];

    // Having the path and changing it for manual/checker testing
    char path[MAX_LEN_PATH];
    sprintf(path, "%s", CHECKER_CL);
    // sprintf(path, "%s", TEST1_CL);
    // sprintf(path, "%s", TEST2_CL);
    // sprintf(path, "%s", TEST3_CL);

    // Getting the rank and size and the dimension of the vector
    // for task 2
    int rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    dim_vect = atoi(argv[1]);

    // Sending the information from 0-3-2-1
    // Then again from 1-2-3-0 so that all the clusters
    // have the correct information

    // If the process if the 0-th cluster
    if (rank == 0) {
        int num_procs, *procs, num_procs_controlled[4];

        // Create the vector to send
        int *v;
        v = (int *)malloc(sizeof(int) * dim_vect);
        for (int i = 0; i < dim_vect; i++) {
            v[i] = dim_vect - i - 1;
        }

        // Filling the filename for the 0-th cluser
        sprintf(filename, "%s%d%s", path, rank, TXT);
        
        // Reading the information from the file
        FILE *fp = fopen(filename, "r");
        fscanf(fp, "%d", &num_procs);

        // Setting the needed parameters
        num_procs_controlled[rank] = num_procs;
        procs = (int *)malloc(num_procs * sizeof(int));

        for (int i = 0; i < num_procs; i++) {
            fscanf(fp, "%d", &procs[i]);
        }
        fclose(fp);

        // Node 0 sends its information to the next node (node 3)
        printf("M(%d,%d)\n", rank, 3);
        MPI_Send(&num_procs, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);

        printf("M(%d,%d)\n", rank, 3);
        MPI_Send(procs, num_procs, MPI_INT, 3, 0, MPI_COMM_WORLD);

        printf("M(%d,%d)\n", rank, 3);
        MPI_Send(&num_procs_controlled, 4, MPI_INT, 3, 0, MPI_COMM_WORLD);

        // Send the vector to the next cluster
        printf("M(%d,%d)\n", rank, 3);
        MPI_Send(v, dim_vect, MPI_INT, 3, 0, MPI_COMM_WORLD);

        // Node 0 receives information from the last node in the ring (node 3)
        MPI_Recv(&num_procs, 1, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        procs = (int *)realloc(procs, num_procs * sizeof(int));
        MPI_Recv(procs, num_procs, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&num_procs_controlled, 4, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Receives the updated vector from the clusters
        MPI_Recv(v, dim_vect, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Printing out the topology
        print_clusters(rank, procs, num_procs_controlled); 

        // Sends to workers the topology
        send_to_workers(rank, procs, num_procs_controlled, v);

        // receive from workers the updated vector
        receive_from_workers(rank, procs, num_procs_controlled, v);

        // print out the vector
        printf("Rezultat:");
        for (int i = 0; i < dim_vect; i++) {
            printf(" %d", v[i]);
        }
        printf("\n");
    }
    
    if (rank == 2 || rank == 3) {
        int num_procs, *procs, num_procs_controlled[4];

        // The vector to send to workers
        int *v;
        v = (int *)malloc(sizeof(int) * dim_vect);

        // Other nodes receive information from the previous node
        MPI_Recv(&num_procs, 1, MPI_INT, (rank + 1) % NUM_CLUSTERS, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        procs = (int *)malloc(num_procs * sizeof(int));
        MPI_Recv(procs, num_procs, MPI_INT, (rank + 1) % NUM_CLUSTERS, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&num_procs_controlled, 4, MPI_INT, (rank + 1) % NUM_CLUSTERS, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        // Receives the updated vector from the clusters
        MPI_Recv(v, dim_vect, MPI_INT, (rank + 1) % NUM_CLUSTERS, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int num_procs_local, *procs_local;
        // Each node reads its own information and adds it to the received information
        sprintf(filename, "%s%d%s", path, rank, TXT);
        FILE *fp = fopen(filename, "r");
        fscanf(fp, "%d", &num_procs_local);
        procs_local = (int *)malloc(num_procs_local * sizeof(int));
        for (int i = 0; i < num_procs_local; i++) {
            fscanf(fp, "%d", &procs_local[i]);
        }
        fclose(fp);

        // Each node concatenates its local information to the received information
        procs = (int *)realloc(procs, (num_procs + num_procs_local) * sizeof(int));
        for (int i = 0; i < num_procs_local; i++) {
            procs[num_procs + i] = procs_local[i];
        }
        num_procs += num_procs_local;
        num_procs_controlled[rank] = num_procs_local;

        // Each node sends the updated information to the next node
        printf("M(%d,%d)\n", rank, rank - 1);
        MPI_Send(&num_procs, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);

        printf("M(%d,%d)\n", rank, rank - 1);
        MPI_Send(procs, num_procs, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);

        printf("M(%d,%d)\n", rank, rank - 1);
        MPI_Send(&num_procs_controlled, 4, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);

        // Send the vector to the next cluster
        printf("M(%d,%d)\n", rank, rank - 1);
        MPI_Send(v, dim_vect, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);

        // receives the topology
        MPI_Recv(&num_procs, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        procs = (int *)realloc(procs, num_procs * sizeof(int));
        MPI_Recv(procs, num_procs, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(num_procs_controlled, 4, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Receives the updated vector from the clusters
        MPI_Recv(v, dim_vect, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Printing out the topology
        print_clusters(rank, procs, num_procs_controlled);

        // Sends to workers the topology
        send_to_workers(rank, procs, num_procs_controlled, v);

        // receive from workers the updated vector
        receive_from_workers(rank, procs, num_procs_controlled, v);

        // The current cluster sends the information to the next (2-3, 3-0)
        printf("M(%d,%d)\n", rank, (rank + 1) % NUM_CLUSTERS);
        MPI_Send(&num_procs, 1, MPI_INT, (rank + 1) % NUM_CLUSTERS, 0, MPI_COMM_WORLD);

        printf("M(%d,%d)\n", rank, (rank + 1) % NUM_CLUSTERS);
        MPI_Send(procs, num_procs, MPI_INT, (rank + 1) % NUM_CLUSTERS, 0, MPI_COMM_WORLD);

        printf("M(%d,%d)\n", rank, (rank + 1) % NUM_CLUSTERS);
        MPI_Send(&num_procs_controlled, 4, MPI_INT, (rank + 1) % NUM_CLUSTERS, 0, MPI_COMM_WORLD);

        // Send the vector to the next cluster
        printf("M(%d,%d)\n", rank, (rank + 1) % NUM_CLUSTERS);
        MPI_Send(v, dim_vect, MPI_INT, (rank + 1) % NUM_CLUSTERS, 0, MPI_COMM_WORLD);
    }
    
    if (rank == 1) {
        int num_procs, *procs, num_procs_controlled[4];

        // The vector to send to workers
        int *v;
        v = (int *)malloc(sizeof(int) * dim_vect);

        // Node 1 receive information from the 2nd node
        MPI_Recv(&num_procs, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        procs = (int *)malloc(num_procs * sizeof(int));
        MPI_Recv(procs, num_procs, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&num_procs_controlled, 4, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Receives the updated vector from the clusters
        MPI_Recv(v, dim_vect, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int num_procs_local, *procs_local;
        // Node 1 reads the information from its file
        sprintf(filename, "%s%d%s", path, rank, TXT);
        FILE *fp = fopen(filename, "r");
        fscanf(fp, "%d", &num_procs_local);
        procs_local = (int *)malloc(num_procs_local * sizeof(int));
        for (int i = 0; i < num_procs_local; i++) {
            fscanf(fp, "%d", &procs_local[i]);
        }
        fclose(fp);

        // Each node concatenates its local information to the received information
        procs = (int *)realloc(procs, (num_procs + num_procs_local) * sizeof(int));
        for (int i = 0; i < num_procs_local; i++) {
            procs[num_procs + i] = procs_local[i];
        }
        num_procs += num_procs_local;
        num_procs_controlled[rank] = num_procs_local;

        // Printing out the topology
        print_clusters(rank, procs, num_procs_controlled);

        // Sends to workers the topology and information
        send_to_workers(rank, procs, num_procs_controlled, v);

        // receive from workers the updated vector
        receive_from_workers(rank, procs, num_procs_controlled, v);

        // Each node sends the updated information to the next node
        printf("M(%d,%d)\n", rank, 2);
        MPI_Send(&num_procs, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);

        printf("M(%d,%d)\n", rank, 2);
        MPI_Send(procs, num_procs, MPI_INT, 2, 0, MPI_COMM_WORLD);

        printf("M(%d,%d)\n", rank, 2);
        MPI_Send(&num_procs_controlled, 4, MPI_INT, 2, 0, MPI_COMM_WORLD);

        // Send the vector to the next cluster
        printf("M(%d,%d)\n", rank, 2);
        MPI_Send(v, dim_vect, MPI_INT, 2, 0, MPI_COMM_WORLD);
    }

    // The workers receive the information
    if (rank > 3) {
        receive_for_workers(rank);
    }

    MPI_Finalize();
    return 0;
}

void print_clusters(int rank, int *procs, int num_procs_controlled[4]) {
    printf("%d -> ", rank);
    int index;

    // Printing for the 0-th node
    printf("0:");

    index = 0;
    if (index < num_procs_controlled[0]) {
        // Print out the first one
        printf("%d", procs[0]);

        for (int i = index + 1; i < num_procs_controlled[0]; i++) {
            // Print out the rest with a "," before
            printf(",%d", procs[i]);
        }
    }

    // After printing all the processes of a cluster, print out a space
    printf(" ");

    // Printing for the 1-st node
    printf("1:");
    index = num_procs_controlled[0] + num_procs_controlled[3] + num_procs_controlled[2];
    if (index < index + num_procs_controlled[1]) {
        // Print out the first one
        printf("%d", procs[index]);

        for (int i = index + 1; i < index + num_procs_controlled[1]; i++) {
            // Print out the rest with a "," before
            printf(",%d", procs[i]);
        }
    }

    // After printing all the processes of a cluster, print out a space
    printf(" ");

    // Printing for the 2-nd node
    printf("2:");
    index = num_procs_controlled[0] + num_procs_controlled[3];
    if (index < index + num_procs_controlled[2]) {
        // Print out the first one
        printf("%d", procs[index]);

        for (int i = index + 1; i < index + num_procs_controlled[2]; i++) {
            // Print out the rest with a "," before
            printf(",%d", procs[i]);
        }
    }

    // After printing all the processes of a cluster, print out a space
    printf(" ");

    // Printing for the 3-rd node
    printf("3:");
    index = num_procs_controlled[0];
    if (index < index + num_procs_controlled[3]) {
        // Print out the first one
        printf("%d", procs[index]);

        for (int i = index + 1; i < index + num_procs_controlled[3]; i++) {
            // Print out the rest with a "," before
            printf(",%d", procs[i]);
        }
    }

    // After printing all the processes of a cluster, print out a space
    printf(" ");

    // At the end, new line
    printf("\n");
}

void receive_for_workers(int rank) {
    int num_procs_controlled[4], *procs, *pointer_num_procs_controlled;
    int total_num_procs, cluster;
    int *v, dim_chunk;

    // Recieving the information from it's leader
    pointer_num_procs_controlled = (int *)malloc(NUM_CLUSTERS * sizeof(int));
    MPI_Recv(&cluster, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&total_num_procs, 1, MPI_INT, cluster, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    procs = (int *)malloc(total_num_procs * sizeof(int));
    MPI_Recv(procs, total_num_procs, MPI_INT, cluster, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(pointer_num_procs_controlled, NUM_CLUSTERS, MPI_INT, cluster, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // receive a chunk of the vector from the leader
    MPI_Recv(&dim_chunk, 1, MPI_INT, cluster, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    v = (int *)malloc(dim_chunk * sizeof(int));
    MPI_Recv(v, dim_chunk, MPI_INT, cluster, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    for (int i = 0; i < 4; i++) {
        num_procs_controlled[i] = pointer_num_procs_controlled[i];
    }

    // Printing out the information
    print_clusters(rank, procs, num_procs_controlled);

    // Calculate 5 * the elements of the vector
    for (int i = 0; i < dim_chunk; i++) {
        v[i] *= 5;
    }

    // Send back to the leader the updated vector chunk
    printf("M(%d,%d)\n", rank, cluster);
    MPI_Send(v, dim_chunk, MPI_INT, cluster, 0, MPI_COMM_WORLD);
}

void send_to_workers(int rank, int *procs, int *num_procs_controlled, int *v) {
    int index;
    int total_num_procs = num_procs_controlled[0] + num_procs_controlled[1] +
                        num_procs_controlled[2] + num_procs_controlled[3];

    switch(rank) {
        case 0:
            index = 0;
            for (int i = index; i < num_procs_controlled[0]; i++) {
                printf("M(%d,%d)\n", 0, procs[i]);
                MPI_Send(&rank, 1, MPI_INT, procs[i], 0, MPI_COMM_WORLD);

                printf("M(%d,%d)\n", 0, procs[i]);
                MPI_Send(&total_num_procs, 1, MPI_INT, procs[i], 0, MPI_COMM_WORLD);

                printf("M(%d,%d)\n", 0, procs[i]);
                MPI_Send(procs, total_num_procs, MPI_INT, procs[i], 0, MPI_COMM_WORLD);

                printf("M(%d,%d)\n", 0, procs[i]);
                MPI_Send(num_procs_controlled, 4, MPI_INT, procs[i], 0, MPI_COMM_WORLD);

                //Send the vector to the workers
                int start = (procs[i] - 4) * (double)dim_vect / (size - NUM_CLUSTERS);
                int end = min((procs[i] - 3) * (double)dim_vect / (size - NUM_CLUSTERS), dim_vect);
                int chunk_size = end - start;

                printf("M(%d,%d)\n", 0, procs[i]);
                MPI_Send(&chunk_size, 1, MPI_INT, procs[i], 0, MPI_COMM_WORLD);

                printf("M(%d,%d)\n", 0, procs[i]);
                MPI_Send(&v[start], chunk_size, MPI_INT, procs[i], 0, MPI_COMM_WORLD);
            }
            break;

        case 1:
            index = num_procs_controlled[0] + num_procs_controlled[3] + num_procs_controlled[2];
            for (int i = index; i < index + num_procs_controlled[1]; i++) {
                printf("M(%d,%d)\n", 1, procs[i]);
                MPI_Send(&rank, 1, MPI_INT, procs[i], 0, MPI_COMM_WORLD);

                printf("M(%d,%d)\n", 1, procs[i]);
                MPI_Send(&total_num_procs, 1, MPI_INT, procs[i], 0, MPI_COMM_WORLD);

                printf("M(%d,%d)\n", 1, procs[i]);
                MPI_Send(procs, total_num_procs, MPI_INT, procs[i], 0, MPI_COMM_WORLD);

                printf("M(%d,%d)\n", 1, procs[i]);
                MPI_Send(num_procs_controlled, 4, MPI_INT, procs[i], 0, MPI_COMM_WORLD);

                //Send the vector to the workers
                int start = (procs[i] - 4) * (double)dim_vect / (size - NUM_CLUSTERS);
                int end = min((procs[i] - 3) * (double)dim_vect / (size - NUM_CLUSTERS), dim_vect);
                int chunk_size = end - start;

                printf("M(%d,%d)\n", 1, procs[i]);
                MPI_Send(&chunk_size, 1, MPI_INT, procs[i], 0, MPI_COMM_WORLD);

                printf("M(%d,%d)\n", 1, procs[i]);
                MPI_Send(&v[start], chunk_size, MPI_INT, procs[i], 0, MPI_COMM_WORLD);
            }
            break;
        
        case 2:
            index = num_procs_controlled[0] + num_procs_controlled[3];
            for (int i = index; i < index + num_procs_controlled[2]; i++) {
                printf("M(%d,%d)\n", 2, procs[i]);
                MPI_Send(&rank, 1, MPI_INT, procs[i], 0, MPI_COMM_WORLD);

                printf("M(%d,%d)\n", 2, procs[i]);
                MPI_Send(&total_num_procs, 1, MPI_INT, procs[i], 0, MPI_COMM_WORLD);

                printf("M(%d,%d)\n", 2, procs[i]);
                MPI_Send(procs, total_num_procs, MPI_INT, procs[i], 0, MPI_COMM_WORLD);

                printf("M(%d,%d)\n", 2, procs[i]);
                MPI_Send(num_procs_controlled, 4, MPI_INT, procs[i], 0, MPI_COMM_WORLD);

                
                //Send the vector to the workers
                int start = (procs[i] - 4) * (double)dim_vect / (size - NUM_CLUSTERS);
                int end = min((procs[i] - 3) * (double)dim_vect / (size - NUM_CLUSTERS), dim_vect);
                int chunk_size = end - start;

                printf("M(%d,%d)\n", 2, procs[i]);
                MPI_Send(&chunk_size, 1, MPI_INT, procs[i], 0, MPI_COMM_WORLD);

                printf("M(%d,%d)\n", 2, procs[i]);
                MPI_Send(&v[start], chunk_size, MPI_INT, procs[i], 0, MPI_COMM_WORLD);
            }
            break;

        case 3:
            index = num_procs_controlled[0];
            for (int i = index; i < index + num_procs_controlled[3]; i++) {
                printf("M(%d,%d)\n", 3, procs[i]);
                MPI_Send(&rank, 1, MPI_INT, procs[i], 0, MPI_COMM_WORLD);

                printf("M(%d,%d)\n", 3, procs[i]);
                MPI_Send(&total_num_procs, 1, MPI_INT, procs[i], 0, MPI_COMM_WORLD);

                printf("M(%d,%d)\n", 3, procs[i]);
                MPI_Send(procs, total_num_procs, MPI_INT, procs[i], 0, MPI_COMM_WORLD);

                printf("M(%d,%d)\n", 3, procs[i]);
                MPI_Send(num_procs_controlled, 4, MPI_INT, procs[i], 0, MPI_COMM_WORLD);

                
                //Send the vector to the workers
                int start = (procs[i] - 4) * (double)dim_vect / (size - NUM_CLUSTERS);
                int end = min((procs[i] - 3) * (double)dim_vect / (size - NUM_CLUSTERS), dim_vect);
                int chunk_size = end - start;

                printf("M(%d,%d)\n", 3, procs[i]);
                MPI_Send(&chunk_size, 1, MPI_INT, procs[i], 0, MPI_COMM_WORLD);

                printf("M(%d,%d)\n", 3, procs[i]);
                MPI_Send(&v[start], chunk_size, MPI_INT, procs[i], 0, MPI_COMM_WORLD);
            }
            break;
    }
}

void receive_from_workers(int rank, int *procs, int num_procs_controlled[4], int *v) {
    // TODO : FOR EACH CLUSTER receive AND UPDATE THE VECTOR
    int index;

    switch(rank) {
        case 0:
            index = 0;
            for (int i = index; i < num_procs_controlled[0]; i++) {
                //Recalculate the chunk size to receive
                int start = (procs[i] - 4) * (double)dim_vect / (size - NUM_CLUSTERS);
                int end = min((procs[i] - 3) * (double)dim_vect / (size - NUM_CLUSTERS), dim_vect);
                int chunk_size = end - start;

                // receive the updated vector from the workers
                MPI_Recv(&v[start], chunk_size, MPI_INT, procs[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            break;

        case 1:
            index = num_procs_controlled[0] + num_procs_controlled[3] + num_procs_controlled[2];
            for (int i = index; i < index + num_procs_controlled[1]; i++) {
                //Recalculate the chunk size to receive
                int start = (procs[i] - 4) * (double)dim_vect / (size - NUM_CLUSTERS);
                int end = min((procs[i] - 3) * (double)dim_vect / (size - NUM_CLUSTERS), dim_vect);
                int chunk_size = end - start;

                // receive the updated vector from the workers
                MPI_Recv(&v[start], chunk_size, MPI_INT, procs[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            break;
        
        case 2:
            index = num_procs_controlled[0] + num_procs_controlled[3];
            for (int i = index; i < index + num_procs_controlled[2]; i++) {
                //Recalculate the chunk size to receive
                int start = (procs[i] - 4) * (double)dim_vect / (size - NUM_CLUSTERS);
                int end = min(((procs[i] - 3)) * (double)dim_vect / (size - NUM_CLUSTERS), dim_vect);
                int chunk_size = end - start;

                // receive the updated vector from the workers
                MPI_Recv(&v[start], chunk_size, MPI_INT, procs[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            break;

        case 3:
            index = num_procs_controlled[0];
            for (int i = index; i < index + num_procs_controlled[3]; i++) {
                //Recalculate the chunk size to receive
                int start = (procs[i] - 4) * (double)dim_vect / (size - NUM_CLUSTERS);
                int end = min((procs[i] - 3) * (double)dim_vect / (size - NUM_CLUSTERS), dim_vect);
                int chunk_size = end - start;

                // receive the updated vector from the workers
                MPI_Recv(&v[start], chunk_size, MPI_INT, procs[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            break;
    }
}
