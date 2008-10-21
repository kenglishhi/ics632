#include <unistd.h>
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <sys/time.h>

// for L : write a program to measure -- L Time to send a measure = L + b * (message size) where message size = 0, send one to the other and then divide by 2
// for b : write a program to bandwidth -- send a big message using b above
// for w : write a program to measure w -- Time to send a measure = L + b * (message size)
//

double get_time_diff(struct timeval *start, struct timeval *finish) ;
int main(int argc, char **argv) {
    int rank, nprocs;
    // Initialize the MPI, get the size and the rank.
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    if (nprocs < 2) {
       printf("Nprocs must be 2 or above\n");
       return -1;
    } 
   
    int buffer[1];
    double latency =0.0;

    struct timeval transmit_start;
    struct timeval transmit_finish;

    buffer[0]=2;

    MPI_Status status ;

    if (rank == 0) { 
      gettimeofday(&transmit_start,NULL); 
      if (MPI_Send(&buffer, 0, MPI_INT, 1, 0, MPI_COMM_WORLD) != MPI_SUCCESS) { 
         fprintf(stderr,"Error while calling MPI_Send()\n");
      }  
      if (MPI_Recv(&buffer, 0, MPI_INT, 1, 0, MPI_COMM_WORLD,&status) != MPI_SUCCESS) { 
         fprintf(stderr,"Error while calling MPI_Recv()\n");
      }  
      gettimeofday(&transmit_finish,NULL); 
      printf("RANK,%d,CompleteTime,%.4f\n", rank, get_time_diff(&transmit_start, &transmit_finish) ) ;
      latency = get_time_diff(&transmit_start, &transmit_finish)/2.0 ; 
      printf("RANK,%d,Latency,%.4f\n", rank, latency ) ;
    } else if (rank == 1) {
      if (MPI_Recv(&buffer, 0, MPI_INT, 0, 0, MPI_COMM_WORLD,&status) != MPI_SUCCESS) { 
         fprintf(stderr,"Error while calling MPI_Recv()\n");
      }  
      if (MPI_Send(&buffer, 0, MPI_INT, 0, 0, MPI_COMM_WORLD) != MPI_SUCCESS) { 
         fprintf(stderr,"Error while calling MPI_Send()\n");
      }  
    } 
    return 0;

    int *big_buffer ; 

    int message_size = 100;
    big_buffer = (int *) malloc(message_size*  sizeof(int) ) ; 
    int message_size_bits = message_size*  sizeof(int) ; 

    if (rank == 0) { 
//      gettimeofday(&transmit_start,NULL); 
      if (MPI_Send(&big_buffer, message_size, MPI_INT, 1, 0, MPI_COMM_WORLD) != MPI_SUCCESS) { 
         fprintf(stderr,"Error while calling MPI_Send()\n");
      }  
      if (MPI_Recv(&big_buffer, message_size-1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD,&status) != MPI_SUCCESS) { 
         fprintf(stderr,"Error while calling MPI_Recv()\n");
      }  
//      gettimeofday(&transmit_finish,NULL); 
//      printf("RANK,%d,Bandwidth,%.4f\n", rank, get_time_diff(&transmit_start, &transmit_finish) ) ;
//      printf("RANK,,Bandwidth\n" ) ;
//      printf("RANK,%d,message_size_bits,%d\n", rank, message_size_bits ) ;
    } else if (rank == 1) {
      if (MPI_Recv(&big_buffer, message_size , MPI_INT, 0, 0, MPI_COMM_WORLD,&status) != MPI_SUCCESS) { 
         fprintf(stderr,"Error while calling MPI_Recv()\n");
      }  
      if (MPI_Send(&big_buffer, message_size -1 , MPI_DOUBLE, 0, 0, MPI_COMM_WORLD) != MPI_SUCCESS) { 
         fprintf(stderr,"Error while calling MPI_Send()\n");
      }  
    } 

     printf("RANK,%d,message_size_bits,%d\n", rank, message_size_bits ) ;
   
   MPI_Finalize();
   

   return 1;
} 
