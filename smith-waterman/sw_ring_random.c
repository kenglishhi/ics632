/* sw_paralle.c */

#include  <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

#define DEBUG    0

void generate_random_array(int *, int, int); 

int  main(int argc,char *argv[]) { 
  MPI_Init(&argc, &argv);  
  int rank, nprocs;  

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  int seq1_length, seq2_length, seq1_chunk_length, seq2_chunk_length ;
  if (argc < 3) {
     printf("You need 2 arguments\n");
     return 0 ;
  } else {
    if ((sscanf(argv[1],"%d",&seq1_length) != 1) ||
        (sscanf(argv[2],"%d",&seq2_length) != 1)) {
      fprintf(stderr,"Usage: %s <seq1_length> <seq2_length>\n", argv[0] );
      exit(1);
    }

  }

  seq1_chunk_length = seq1_length/nprocs; 
  seq2_chunk_length = seq2_length/nprocs; 

  srand(time(0)) ;
  int i; 
  int *seq1_arr, *seq2_arr, *seq1_chunk_arr, *seq2_chunk_arr;
  if (rank == 0 ) { 
     seq1_arr = (int *) malloc(seq1_length * sizeof(int) )  ;
     seq2_arr = (int *) malloc(seq2_length * sizeof(int) )  ;
     generate_random_array(seq1_arr, seq1_length,  20);   
     generate_random_array(seq2_arr, seq2_length,  20);   
     for (i = 0; i < seq1_length ; i++ ) { 
        printf("[%d] seq1_arr[%d] = %d\n " ,rank, i, *(seq1_arr+i) ) ; 
     } 
  } 

  seq1_chunk_arr = (int *) calloc(seq1_chunk_length , sizeof(int) )  ;
  seq2_chunk_arr = (int *) calloc(seq2_chunk_length , sizeof(int) )  ;

  printf("Rank is not %d, seq1_length: %d, seq1_chunk_length: %d \n", rank,  seq1_length,seq1_chunk_length  );
  
  if (MPI_Scatter(seq1_arr, seq1_chunk_length, MPI_INT, seq1_chunk_arr , seq1_chunk_length,  MPI_INT, 0, MPI_COMM_WORLD) != MPI_SUCCESS ) { 
    printf("Error while calling MPI_Scatter()\n"); 
  } 
  printf("[%d] Back from scatter\n" ,rank ) ; 

  for (i = 0; i < seq1_chunk_length ; i++ ) { 
     printf("[%d] value = %d\n " ,rank, *(seq1_chunk_arr+i) ) ; 
  } 
  printf("[%d] finished scatter \n" ,rank ) ; 


  MPI_Finalize(); 
    
  return 1; 
}
