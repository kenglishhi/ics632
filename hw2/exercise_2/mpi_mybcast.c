#include <unistd.h>
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

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
