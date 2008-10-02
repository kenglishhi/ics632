#include <unistd.h>
#include <stdio.h>
#include <mpi.h>


void MPI_MyBcast(int *buffer, int count, int root, MPI_Comm comm); 
void MPI_MyBcast(int *buffer, int count, int root, MPI_Comm comm) {


}

int main(int argc, char **argv) {
  int my_rank, n; 
  char hostname[128]; 
  MPI_Init(&argc, &argv); 
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
  MPI_Comm_size(MPI_COMM_WORLD, &n); 
  gethostname(hostname, 128); 
  if (my_rank == 0) { 
     printf("I am the master, %d", my_rank); 
  } else {
     printf("I am a worker, %d", my_rank); 


  } 
  int i = 0;
  for (i=0; i< 20; i++) { 
     printf("Hello %d my_rank = %d\n",i, my_rank); 
  } 
  

}
