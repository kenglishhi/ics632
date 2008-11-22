/* sw_paralle.c */

#include  <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

#define DEBUG    0
#define DEBUG   0
#define STRLEN     8
#define ITER       1
#define GAP   -1
#define MATCH 1
#define MISMATCH -1
#define SEQ1_LEN 8
#define DIRECTION_NONE     0
#define DIRECTION_UP       1
#define DIRECTION_LEFT     2
#define DIRECTION_DIAGONAL 3
#define ROOT 0


void generate_random_array(int *, int, int); 

int  main(int argc,char *argv[]) { 
  /* Initialize MPI */ 
  MPI_Init(&argc, &argv);  
  int rank, nprocs;  

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  /* Parse Command Line Args */ 
  int seq1_length, seq2_length, nrows, ncols ;
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

  /*  Check for current constraints of the program */ 

  int matrix_width = seq1_length+1 ; 

  if (seq1_length != seq2_length) {
    printf("Current program limitation, seq1_length != seq2_length \n");
    return 1;
  }

  if (((matrix_width) % nprocs) != 0 ) {
    printf("Current program limitation, matrix_width (%d) must be divisible by nprocs (%d)  \n", (matrix_width), nprocs );
    return 1;
  }

  if (((matrix_width) % 2) != 0 ) {
    printf("Current program limitation, matrix_width (%d) must be 2 (%d)  \n", (matrix_width), nprocs );
    return 1;
  }

  int chunk_size = 2; // (matrix_width)/2;
  if (((matrix_width) % chunk_size) != 0 ) {
    printf("Current program limitation, matrix_width (%d) must be 2 (%d)  \n", (matrix_width), nprocs );
    return 1;
  }


  /*  Set the Chunk Length */ 

  nrows = seq2_length / nprocs; 

  srand(time(0)) ;
  int i,j; 
  int *seq1_arr, *seq2_arr;
  // all processes will have a copy of Sequence 1
  // They will have a slice of sequence 2
  seq1_arr = (int *) malloc(seq1_length * sizeof(int) )  ;
  seq2_arr = (int *) malloc(seq2_length * sizeof(int) )  ;

  if (rank == 0 ) { 
     generate_random_array(seq1_arr, seq1_length,  20);   
     for (i = 0; i < seq1_length ; i++ ) { 
        seq2_arr[i] = *seq1_arr; 
     } 
//     generate_random_array(seq2_arr, seq2_length,  20);   
//     for (i = 0; i < seq1_length ; i++ ) { 
//        printf("[%d] seq1_arr[%d] = %d\n " ,rank, i, *(seq1_arr+i) ) ; 
//     } 
  } 


  printf("Rank is not %d, seq2_length: %d, ncols: %d, nrows: %d \n", rank,  seq2_length, matrix_width, nrows   );
  if (MPI_Bcast(seq1_arr, seq1_length, MPI_INT, ROOT, MPI_COMM_WORLD) ) {
    printf("Error while calling MPI_Bcast()\n"); 
    exit(0); 
  } 

//  for (i = 0; i < seq1_length ; i++ ) { 
//     printf("[%d] seq1_arr[%d] = %d --  \n" ,rank,i, *(seq1_arr+i) ) ; 
//  } 
//  printf("\n" ) ; 

  if (MPI_Bcast(seq2_arr, seq2_length, MPI_INT, ROOT, MPI_COMM_WORLD) ) {
    printf("Error while calling MPI_Bcast()\n"); 
    exit(0); 
  } 

//  for (i = 0; i < seq2_length ; i++ ) { 
//     printf("[%d] seq2_arr[%d] = %d --  \n" ,rank,i, *(seq2_arr+i) ) ; 
//  } 
//  printf("\n" ) ; 
  
/*  if (MPI_Scatter(seq2_arr, seq2_chunk_length, MPI_INT, seq2_chunk_arr , seq2_chunk_length,  MPI_INT, 0, MPI_COMM_WORLD) != MPI_SUCCESS ) { 
    printf("Error while calling MPI_Scatter()\n"); 
  } 

  printf("[%d] Back from scatter\n" ,rank ) ; 

  for (i = 0; i < seq2_chunk_length ; i++ ) { 
     printf("[%d] value = %d\n " ,rank, *(seq2_chunk_arr+i) ) ; 
  } 
*/

  int score_matrix[matrix_width/nprocs][(matrix_width)] ;
  int direction_matrix[matrix_width/nprocs][(matrix_width)] ;

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

  int number_of_chucks= matrix_width / chunk_size;
  int chunk;
  int prev_row_buffer[matrix_width] ;
  int src, dest;
  int max_i, max_j, max_score;
  if (rank == ROOT ) {
     dest = rank + 1 ;
     for (chunk = 0; chunk < number_of_chucks; chunk++ ) {
       calculate_chunk(&seq1_arr[0], &seq2_arr[0], &score_matrix[0][0], &direction_matrix[0][0], &prev_row_buffer[0],  nrows, matrix_width, chunk*chunk_size, chunk_size, &max_score, &max_i, &max_j);
       if (DEBUG)
         printf("{%d} Sending columns %d \n", rank, chunk*chunk_size) ;
       MPI_Send(&score_matrix[nrows-1][chunk*chunk_size], chunk_size, MPI_INT,  dest, 0, MPI_COMM_WORLD);
     }
     print_score_matrix(&score_matrix[0][0],  nrows, matrix_width  );
     printf("Max_score: %d, max_i: %d, max_j: %d \n", max_score, max_i, max_j);
  } else if (rank == (nprocs-1))  {
     src = rank - 1 ;
     MPI_Status status ;

     for (chunk = 0; chunk< number_of_chucks; chunk++ ) {
       if (DEBUG)
         printf("{%d} Receiving columns %d \n", rank, chunk*chunk_size) ;
       MPI_Recv(&prev_row_buffer[chunk*chunk_size], chunk_size, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
       calculate_chunk(&seq1_arr[0], &seq2_arr[0], &score_matrix[0][0], &direction_matrix[0][0], &prev_row_buffer[0],  nrows, matrix_width, chunk*chunk_size , chunk_size, &max_score, &max_i, &max_j);
     }
     print_score_matrix(&score_matrix[0][0],  nrows, matrix_width  );
     printf("Max_score: %d, max_i: %d, max_j: %d \n", max_score, max_i, max_j);

  } else {
     src = rank - 1 ;
     dest = rank + 1 ;
     if (DEBUG)
       printf("{%d} Else stuff, src:%d, dest: %d  \n", rank, src, dest) ;
     MPI_Status status ;

     for (chunk = 0; chunk< number_of_chucks; chunk++ ) {
       if (DEBUG)
         printf("{%d} Receiving columns %d \n", rank, chunk*chunk_size) ;
       MPI_Recv(&prev_row_buffer[chunk*chunk_size], chunk_size, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
//       for (i=(chunk*chunk_size) ; i< (chunk*chunk_size+chunk_size) ; i ++) {
//          printf("{%d} GOT VALUE %d \n", rank, prev_row_buffer[i]) ;
//       }
       calculate_chunk(&seq1_arr[0], &seq2_arr[0], &score_matrix[0][0], &direction_matrix[0][0], &prev_row_buffer[0],  nrows, matrix_width, chunk*chunk_size , chunk_size, &max_score, &max_i, &max_j);
       if (DEBUG)
         printf("{%d} Sending columns %d \n", rank, chunk*chunk_size) ;
       MPI_Send(&score_matrix[nrows-1][chunk*chunk_size], chunk_size, MPI_INT,  dest, 0, MPI_COMM_WORLD);
     }
     print_score_matrix(&score_matrix[0][0],  nrows, matrix_width  );
     printf("Max_score: %d, max_i: %d, max_j: %d \n", max_score, max_i, max_j);

  }








  MPI_Finalize(); 
    
  return 1; 
}
