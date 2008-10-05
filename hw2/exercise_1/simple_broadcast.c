#include <unistd.h>
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>



void MPI_MyBcast(int *buffer, int count, int root, MPI_Comm comm); 
void MPI_MyBcast(int *buffer, int count, int root, MPI_Comm comm) {
  int  proc; int tag=0;
  MPI_Request request ; 
  printf("Called Broadcast, size = %d\n",count); 
  int nprocs;
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
  int x = 666;
  for (proc=0; proc<nprocs ; proc++) { 
      printf("Doing isend to proc = %d\n",proc); 
//      MPI_Isend(buffer, count, MPI_INT, proc, tag, comm, &request); 
      MPI_Isend(buffer, count, MPI_INT, proc, tag, comm, &request); 
  } 
  printf("Finished broad-casting %d\n",proc); 
//  MPI_Isend(buffer, count, MPI_INT, 1, tag, comm, request); 

}

int main(int argc, char **argv) {
  int rank, nprocs;  int master = 0; 
  int tag = 0 ;
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

  gethostname(hostname, 128); 
  if (rank == master) { 
     printf("I am the master, %d\n", rank); 
     a = (int *) malloc(array_size * sizeof(int));
     for (i=0; i< array_size; i++) { 
        a[i] = (int) rand();
        printf("a[%d] =  %d \n",i, a[i]); 
     } 
     MPI_MyBcast(a, array_size, rank, MPI_COMM_WORLD);
  } else {
//     printf("I am a worker: %d\n", rank); 
  } 

  int *x  ; 
  x = (int *) malloc(array_size * sizeof(int));
  MPI_Status status;
  MPI_Request request;
  MPI_Recv(x, array_size, MPI_INT, master, tag, MPI_COMM_WORLD, &status); 

  MPI_Isend(x, array_size, MPI_INT, master, tag, MPI_COMM_WORLD, &request); 

  if (rank == 0) { 
     int proc;
     for (proc=0; proc<nprocs ; proc++) { 
        MPI_Recv(x, array_size, MPI_INT, proc, tag, MPI_COMM_WORLD, &status); 
        for (i=0; i < array_size; i++) { 
           if (a[i] == x[i]) { 
             printf("%d ACK x[%d] == a[%d] == %d\n", proc, i, i,  x[i]);
           }  else { 
             printf("%d CORRUPT x[%d] == a[%d] == %d,%d \n", proc, i, i,  x[i], a[i]);
           } 
        } 
     } 
 
  } 

  
  MPI_Finalize(); 

}
