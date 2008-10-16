#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>


void Ring_Send(int *buffer, int length) { 
    int rank, nprocs, dest; 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    if (rank == (nprocs -1)) { 
       dest = 0 ;
    }  else { 
       dest = rank + 1; 
    } 
/*
   int i;
   for (i = 0; i < length; i++) {
       printf("%d\t-> %d, SendBuffer[%d] = %d\n", rank,dest, i, buffer[i] );
    }
*/

    MPI_Send(buffer, length, MPI_INT,  dest, 0, MPI_COMM_WORLD); 
} 


void Ring_Recv(int *buffer, int length) { 

    int rank, nprocs, src; 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Status status ; 
    if (rank == 0 )  { 
       src = nprocs -1 ;
    }  else { 
       src = rank - 1; 
    } 
    MPI_Recv(buffer, length, MPI_INT, src, 0, MPI_COMM_WORLD, &status); 

/*

    int i; 
    for (i = 0; i < length; i++) {
       printf("%d\t<- %d, RecvBuffer[%d] = %d\n", rank,src, i, buffer[i]);
    }
*/
} 



