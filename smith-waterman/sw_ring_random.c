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

  if (seq1_length != seq2_length) {
    printf("Current program limitation, seq1_length != seq2_length \n");
    return 1;
  }
  if (((seq1_length+1) % nprocs) != 0 ) {
    printf("Current program limitation, seq1_length+1 (%d) must be divisible by nprocs (%d)  \n", (seq1_length+1), nprocs );
    return 1;
  }
  if (((seq1_length+1) % 2) != 0 ) {
    printf("Current program limitation, seq1_length+1 (%d) must be 2 (%d)  \n", (seq1_length+1), nprocs );
    return 1;
  }
  int chunk_size = 2; // (seq1_length+1)/2;
  if (((seq1_length+1) % chunk_size) != 0 ) {
    printf("Current program limitation, seq1_length+1 (%d) must be 2 (%d)  \n", (seq1_length+1), nprocs );
    return 1;
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


  int score_matrix[(seq2_length+1)/nprocs][(seq1_length+1)] ;
  int direction_matrix[(seq2_length+1)/nprocs][(seq1_length+1)] ;

  if (rank ==0) {
    for (j=0; j <= seq1_length; j++) {
      score_matrix[0][j] = 0;
      direction_matrix[0][j]   = DIRECTION_NONE;
    }
  }

  for (i=0; i < (seq2_length+1)/nprocs; i++ ) {
    score_matrix[i][0] = 0;
    direction_matrix[i][0]   = DIRECTION_NONE;
  }

  int chunk;
  int prev_row_buffer[ncols] ;
  int src, dest;
  int max_i, max_j, max_score;
  if (rank ==0 ) {
     dest = rank + 1 ;
     for (chunk = 0; chunk< number_of_chucks; chunk++ ) {
       calculate_chunk(&seq1_arr[0], &seq2_arr[0], &score_matrix[0][0], &direction_matrix[0][0], &prev_row_buffer[0],  nrows, ncols, chunk*chunk_size, chunk_size, &max_score, &max_i, &max_j);
       if (DEBUG)
         printf("{%d} Sending columns %d \n", rank, chunk*chunk_size) ;
       MPI_Send(&score_matrix[nrows-1][chunk*chunk_size], chunk_size, MPI_INT,  dest, 0, MPI_COMM_WORLD);
     }
     print_score_matrix(&score_matrix[0][0],  nrows, ncols  );
     printf("Max_score: %d, max_i: %d, max_j: %d \n", max_score, max_i, max_j);
  } else if (rank == (nprocs-1))  {
     src = rank - 1 ;
     MPI_Status status ;

     for (chunk = 0; chunk< number_of_chucks; chunk++ ) {
       if (DEBUG)
         printf("{%d} Receiving columns %d \n", rank, chunk*chunk_size) ;
       MPI_Recv(&prev_row_buffer[chunk*chunk_size], chunk_size, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
       calculate_chunk(&seq1_arr[0], &seq2_arr[0], &score_matrix[0][0], &direction_matrix[0][0], &prev_row_buffer[0],  nrows, ncols, chunk*chunk_size , chunk_size, &max_score, &max_i, &max_j);
     }
     print_score_matrix(&score_matrix[0][0],  nrows, ncols  );
     printf("Max_score: %d, max_i: %d, max_j: %d \n", max_score, max_i, max_j);

  } else{
     src = rank - 1 ;
     dest = rank + 1 ;
     if (DEBUG)
       printf("{%d} Else stuff, src:%d, dest: %d  \n", rank, src, dest) ;
     MPI_Status status ;

     for (chunk = 0; chunk< number_of_chucks; chunk++ ) {
       if (DEBUG)
         printf("{%d} Receiving columns %d \n", rank, chunk*chunk_size) ;
       MPI_Recv(&prev_row_buffer[chunk*chunk_size], chunk_size, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
       for (i=(chunk*chunk_size) ; i< (chunk*chunk_size+chunk_size) ; i ++) {
          printf("{%d} GOT VALUE %d \n", rank, prev_row_buffer[i]) ;
       }
       calculate_chunk(&seq1_arr[0], &seq2_arr[0], &score_matrix[0][0], &direction_matrix[0][0], &prev_row_buffer[0],  nrows, ncols, chunk*chunk_size , chunk_size, &max_score, &max_i, &max_j);
       if (DEBUG)
         printf("{%d} Sending columns %d \n", rank, chunk*chunk_size) ;
       MPI_Send(&score_matrix[nrows-1][chunk*chunk_size], chunk_size, MPI_INT,  dest, 0, MPI_COMM_WORLD);
     }
     print_score_matrix(&score_matrix[0][0],  nrows, ncols  );
     printf("Max_score: %d, max_i: %d, max_j: %d \n", max_score, max_i, max_j);

  }








  MPI_Finalize(); 
    
  return 1; 
}
