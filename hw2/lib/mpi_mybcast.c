#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>

#define MASTER_RANK 0
#define DEFAULT_TAG 0
#define DEBUG 1

void MPI_MyBcast(int *buffer, int count, int root, MPI_Comm comm) {
    int nprocs, proc, rank;
    if (DEBUG) {
        printf("Called MPI_MyBcast(buffer, %d, %d ,comm)\n", count, root );
    }

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm,&nprocs);

    MPI_Request requests[nprocs]  ;
    MPI_Status status  ;

    if (rank == root) {
        for (proc=0; proc<nprocs ; proc++) {
            if (DEBUG) {
                printf("Doing isend to proc = %d\n",proc);
            }
            MPI_Isend(buffer, count, MPI_INT, proc, DEFAULT_TAG, comm, &requests[proc]);
        }
   }

   MPI_Recv(buffer, count, MPI_INT, root, DEFAULT_TAG, comm, &status);
}

void randomize_array(int *array, int array_size) {
    int i ;
    for (i=0; i< array_size; i++)
        array[i] = rand();
}

void print_array(int *array, int array_size, int rank  ) {
    int i ;
    for (i=0; i< array_size; i++) {
        printf("Process #%d array[%d] =  %d \n",rank, i, array[i]);
    }
}

