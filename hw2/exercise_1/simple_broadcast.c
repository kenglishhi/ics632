#include <unistd.h>
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define MASTER_RANK 0
#define DEFAULT_TAG 0


void MPI_MyBcast(int *buffer, int count, int root, MPI_Comm comm);
void MPI_MyBcast(int *buffer, int count, int root, MPI_Comm comm) {
    int proc;
    printf("Called Broadcast, size = %d\n",count);
    int nprocs;
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Request requests[nprocs]  ;
    for (proc=0; proc<nprocs ; proc++) {
	printf("Doing isend to proc = %d\n",proc);
//      MPI_Isend(buffer, count, MPI_INT, proc, DEFAULT_TAG, comm, &request);
	MPI_Isend(buffer, count, MPI_INT, proc, DEFAULT_TAG, comm, &requests[proc]);
    }
    printf("Finished broad-casting %d\n",proc);
    MPI_Wait(&requests[0] , MPI_STATUS_IGNORE ); 
    printf("Done waiting %d\n",proc);
//  MPI_Isend(buffer, count, MPI_INT, 1, DEFAULT_TAG, comm, request);

}
void master_send_broadcast(int *array, int array_size, MPI_Comm comm)  {
    int i;
    for (i=0; i< array_size; i++) {
	array[i] = (int) rand();
	printf("array[%d] =  %d \n",i, array[i]);
    }
    MPI_MyBcast(array, array_size, MASTER_RANK, comm);
}
void worker_receive_broadcast(int *array, int array_size, MPI_Comm comm) {
    int rank, i;
    MPI_Status status;
    MPI_Recv(array, array_size, MPI_INT, MASTER_RANK, DEFAULT_TAG, comm, &status);
    for (i=0; i< array_size; i++) {
	printf("Process #%d array[%d] =  %d \n",rank, i, array[i]);
    }   
}
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
		printf("%d ACK array[%d] == a[%d] == %d\n", proc, i, i,  array[i]);
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

    int *a ;
    a = (int *) malloc(array_size * sizeof(int));

    gethostname(hostname, 128);
    if (rank == MASTER_RANK) {
	printf("I am the master, %d\n", rank);
	master_send_broadcast(a, array_size, MPI_COMM_WORLD );
    }

    int *x  ;
    x = (int *) malloc(array_size * sizeof(int));
    worker_receive_broadcast(x, array_size, MPI_COMM_WORLD );
    worker_send_data_confirmation(x, array_size, MPI_COMM_WORLD );
    if (rank == MASTER_RANK) {
      int *confirm  ;
      confirm = (int *) malloc(array_size * sizeof(int));
      master_receive_data_confirmation(a, confirm, array_size, MPI_COMM_WORLD );
    }

    MPI_Finalize();
    return 0;

}
