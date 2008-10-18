#include <unistd.h>
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>

void Ring_Send(int *buffer, int length) ; 
void Ring_Recv(int *buffer, int length) ; 


int main(int argc, char **argv) {
    int rank, nprocs;
    // Initialize the MPI, get the size and the rank.
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
   
    int send_buffer[2];  
    int receive_buffer[2];  
    send_buffer[0] = 1.0 * (rank+1); 
    send_buffer[1] = 2.0 * (rank+1); 

    Ring_Send(&send_buffer[0], 2); 
    Ring_Recv(&receive_buffer[0], 2); 

    int i; 
    for (i = 0; i < 2; i++) {
          printf("%d\t Received[%d] = %d\n", rank,i,  receive_buffer[i] );
   
   }
    
    MPI_Finalize();
    return 1;  


} 
