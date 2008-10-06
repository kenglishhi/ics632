#include <unistd.h>
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define MASTER_RANK 0
#define DEFAULT_TAG 0
#define DEBUG 1


void MPI_MyBcast(int *buffer, int count, int root, MPI_Comm comm);
void worker_send_data_confirmation(int *array, int array_size, MPI_Comm comm) {
    MPI_Request request;
    MPI_Isend(array, array_size, MPI_INT, MASTER_RANK, DEFAULT_TAG, comm, &request);
}

void master_receive_data_confirmation(int *initial_array, int *array, int array_size, MPI_Comm comm ) {
    int proc, nprocs, i ;
    MPI_Status status;
    MPI_Comm_size(comm, &nprocs);
    for (proc=0; proc < nprocs ; proc++) {
	MPI_Recv(array, array_size, MPI_INT, proc, DEFAULT_TAG, comm, &status);
	for (i=0; i < array_size; i++) {
	    if (initial_array[i] == array[i]) {
		if (DEBUG) {
		    printf("%d ACK array[%d] == a[%d] == %d\n", proc, i, i,  array[i]);
		}
	    }  else {
		printf("%d CORRUPT array[%d] == initial_array[%d] == %d,%d \n", proc, i, i,  array[i], initial_array[i]);
	    }
	}
    }
}
int main(int argc, char **argv) {
    int rank, nprocs;
    char hostname[128];
    // Initialize the MPI, get the size and the rank.
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int array_size = atoi(argv[1]);
    int i;
    int seed = 10000;
    srand(seed);

    int *data_array ;
    data_array = (int *) malloc(array_size * sizeof(int));


    int *x  ;
    x = (int *) malloc(array_size * sizeof(int));
    if (rank == MASTER_RANK) {
        randomize_array(data_array, array_size, rank); 
    }
    MPI_MyBcast(data_array, array_size, MASTER_RANK, MPI_COMM_WORLD);
    
    worker_send_data_confirmation(data_array, array_size, MPI_COMM_WORLD );

/*  Each process that had received the array must send it back to the
 *  master so that the master can check the data (making sure that it
 *  receives the same data it sent out in the first place).
 */

    if (rank == MASTER_RANK) {
	int *confirm  ;
	confirm = (int *) malloc(array_size * sizeof(int));
	master_receive_data_confirmation(data_array, confirm, array_size, MPI_COMM_WORLD );
    }
/*
 * The program then does the same thing calling the original
 * MPI_Bcast() function.
 */
    // we randomize the array again
    if (rank == MASTER_RANK) {
	for (i=0; i< array_size; i++) {
	    data_array[i] = (int) rand();
	    if (DEBUG) {
		printf("array[%d] =  %d \n",i, data_array[i]);
	    }
	}
    }

    if (DEBUG) {
	printf("Master going to Bcast \n\n");
    }
    MPI_Bcast(data_array, array_size, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);

    for (i=0; i< array_size; i++) {
	printf("Process #%d array[%d] =  %d \n",rank, i, data_array[i]);
    }
    MPI_Finalize();
    return 0;

}
