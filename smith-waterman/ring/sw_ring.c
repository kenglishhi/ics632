/*************************************
 *
 * sw_ring.c
 * Author: Kevin English
 * University of Hawaii
 * ICS 632
 *
 * This program takes 2 strings and generates the optimal local alignment. It uses a ring to accomplish this.
 **/


#include  <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

#define DEBUG    0
#define STRLEN   8
#define ITER     1
#define GAP   -1
#define MATCH 1
#define MISMATCH -1
#define SEQ1_LEN 8
#define DIRECTION_NONE     0
#define DIRECTION_UP       1
#define DIRECTION_LEFT     2
#define DIRECTION_DIAGONAL 3
#define GLOBAL_ROW_NUMBER(I,RANK,ROW_SIZE) ( (RANK)*(ROW_SIZE)+(I) )
#define ARRAY_OFFSET(I,J,NROWS)  ((I)*(NROWS) + (J))

void calculate_chunk(int *, int *, int *, int *, int *, int,int, int, int, int *, int *, int *) ;
void print_score_matrix(int *, int, int ) ;

int  main(int argc,char *argv[]) {
  MPI_Init(&argc, &argv);
  int rank, nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  char alphabet[21] = "acdefghiklmnpqrstvwy";
  char *seq1 ;
  char *seq2 ;

  if (argc < 3) {
    printf("You need 2 arguments\n");
    return 0 ;
  } else {
    seq1 = argv[1] ;
    seq2 = argv[2] ;
  }


  if (DEBUG) {
    printf("seq1 = %s\n", seq1);
    printf("seq2 = %s\n", seq2);
  }

  int seq1_length = 0 ;
  int seq2_length = 0;
  while (seq1[seq1_length] != '\0') {
    seq1_length++;
  }
  while (seq2[seq2_length] != '\0') {
    seq2_length++;
  }
  if (seq1_length != seq2_length) {
    printf("Current program limitation, seq1_length != seq2_length \n");
    return 1;
  }
//  printf("seq2_length = %d\n", seq2_length );
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


  int nrows =  (seq1_length+1)/nprocs;
  int ncols =  (seq1_length+1);
  int number_of_chucks= (seq1_length+1)/chunk_size;
  if (DEBUG)
    printf("nrows: %d, ncols: %d, chunk_size: %d, number_of_chunks: %d\n", nrows,ncols,chunk_size,number_of_chucks);

//  char *seq1 = "ppeaiccc";
//  char *seq2 = "ggeaicgg";

  int seq1_arr[seq1_length], seq2_arr[seq2_length]  ;
  int i, j;

/* OLD
   int score_matrix[seq2_length+1][seq1_length+1] ;
   int direction_matrix[seq2_length+1][seq1_length+1] ;
 */


//  int max_i=-1, max_j=-1, max_score=-1 ;
//  int diagonal_score, left_score, up_score  ;
//  int letter1, letter2;

  for (i=0; i < seq1_length; i++) {
    for (j=0; j<20; j++) {
      if (seq1[i] == alphabet[j]){
	seq1_arr[i] = j;
      }
    }
  }

  int l;
  for (l=0; l < seq2_length; l++) {
    for (j=0; j<20; j++) {
      if (seq2[l] == alphabet[j]) {
	seq2_arr[l] = j;
      }
    }
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
      calculate_chunk(&seq1_arr[0], &seq2_arr[0], &score_matrix[0][0], &direction_matrix[0][0], &prev_row_buffer[0],  nrows, ncols, chunk*chunk_size, chunk_size, &max_score, &max_i, &max_j);
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
//       for (i=(chunk*chunk_size) ; i< (chunk*chunk_size+chunk_size) ; i ++) {
//          printf("{%d} GOT VALUE %d \n", rank, prev_row_buffer[i]) ;
//       }
      calculate_chunk(&seq1_arr[0], &seq2_arr[0], &score_matrix[0][0], &direction_matrix[0][0], &prev_row_buffer[0],  nrows, ncols, chunk*chunk_size, chunk_size, &max_score, &max_i, &max_j);
      if (DEBUG)
	printf("{%d} Sending columns %d \n", rank, chunk*chunk_size) ;
      MPI_Send(&score_matrix[nrows-1][chunk*chunk_size], chunk_size, MPI_INT,  dest, 0, MPI_COMM_WORLD);
    }
    print_score_matrix(&score_matrix[0][0],  nrows, ncols  );
    //   printf("Max_score: %d, max_i: %d, max_j: %d \n", max_score, max_i, max_j);

  }
  MPI_Finalize();
  return 1;

}
