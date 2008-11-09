/* sw_paralle.c */

#include  <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

#define DEBUG     0
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
#define GLOBAL_ROW_NUMBER(I,RANK,ROW_SIZE) ( (RANK)*(ROW_SIZE)+(I) )
#define ARRAY_OFFSET(I,J,NROWS)  ((I)*(NROWS) + (J))  


void print_score_matrix(int *matrix, int row_size, int col_size) {
   int rank, nprocs;  
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
   int *current_value;
   int i,j;

//   printf("row_size , col_size = [%d,%d]  \n\n", row_size, col_size );
   for (i = 0; i < row_size; i++) {
        printf(" [" );
        for (j = 0; j < col_size; j++){
            current_value = matrix + (i * col_size) + j;
            printf("(%d,%d)=%d ",  GLOBAL_ROW_NUMBER(i,rank,row_size), j, *current_value  );
        }
        printf("] " );

        printf("\n" );
    }
}

void calculate_chunk(int *seq1_arr, int *seq2_arr, int *score_matrix, int *direction_matrix, int *prev_row, int nrows,int ncols, int col_start, int chunk_size, int *max_score, int *max_i, int *max_j ) {
   int rank, nprocs;  
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
//   printf("Calculating chunk= %d, col_start\n", col_start);  

   int row_start=0; 
   int col_end;

   if (GLOBAL_ROW_NUMBER(0,rank,nrows) == 0 ) { 
      row_start=1; 
   }  

   if (col_start ==0 ) {
     col_start=1; 
     col_end = col_start+chunk_size - 1; 
   } else {
     col_end = col_start+chunk_size  ; 
   } 
   if (DEBUG) 
   printf("{%d} row_start: %d, row_end : %d,  col_start: %d,  col_end: %d \n", rank, row_start, nrows, col_start,  col_end); 
   int i,j;
   int  diagonal_score, left_score, up_score;
   int letter1, letter2 ; 
   int *diagonal_ptr,  *up_ptr; 
   for (i = row_start; i < nrows;  i++ ) { 
     for (j = col_start; j < col_end; j++ ) { 
       diagonal_score=0; left_score=0; up_score=0;      
       letter1 = *(seq1_arr+ GLOBAL_ROW_NUMBER((i-1),rank,nrows) ) ; 
       letter2 = *(seq2_arr +(j-1) ); 
       // calculate match score 
       //  
   
       if (i==0) {     
         diagonal_ptr = (prev_row + (j-1)) ; 
         up_ptr =  (prev_row + j  ) ; 
       } else{ 
         diagonal_ptr = (score_matrix + ARRAY_OFFSET(i-1, j-1, ncols)) ; 
         up_ptr =  (score_matrix + ARRAY_OFFSET(i-1, j, ncols)  ) ; 
       } 

       if (letter1 == letter2)  { 
         diagonal_score = *diagonal_ptr  + MATCH; 
       } 
       else { 
         diagonal_score = *diagonal_ptr   + MISMATCH; 
       } 
       up_score = *up_ptr + GAP ; 
       left_score = *(score_matrix + ARRAY_OFFSET(i, j-1, ncols)  )  + GAP;
       if ((diagonal_score <= 0) && (up_score <= 0) && (left_score <= 0)) {
            *(score_matrix + ARRAY_OFFSET(i, j, ncols) )    = 0;
//            printf("[%d,%d] diagonal_score: %d, up_score: %d, left_score: %d, score_matrix:%d \n", i, j, diagonal_score, up_score, left_score, score_matrix[i][j]); 
            *(direction_matrix + ARRAY_OFFSET(i, j, ncols))  = DIRECTION_NONE;
            continue; 
        }

        // choose best score
        if (diagonal_score >= up_score) {
            if (diagonal_score >= left_score) {
              *(score_matrix + ARRAY_OFFSET(i, j, ncols) )    = diagonal_score;
              *(direction_matrix + ARRAY_OFFSET(i, j, ncols)) = DIRECTION_DIAGONAL;
                //$matrix[$i][$j]{pointer} = "diagonal";
            }
            else {
              *(score_matrix + ARRAY_OFFSET(i, j, ncols) )    = left_score;
              *(direction_matrix + ARRAY_OFFSET(i, j, ncols)) = DIRECTION_LEFT;
//              direction_matrix[i][j]   = DIRECTION_LEFT;
                //$matrix[$i][$j]{pointer} = "left";
            }
        } else {
            if (up_score >= left_score) {
              *(score_matrix + ARRAY_OFFSET(i, j, ncols) )   = up_score;
              *(direction_matrix + ARRAY_OFFSET(i, j, ncols)) = DIRECTION_UP;
//              direction_matrix[i][j]   = DIRECTION_UP;
                //$matrix[$i][$j]{pointer} = "up";
            }
            else {
              *(score_matrix + ARRAY_OFFSET(i, j, ncols) )    = left_score;
              *(direction_matrix + ARRAY_OFFSET(i, j, ncols)) = DIRECTION_LEFT ;
//              direction_matrix[i][j]   = DIRECTION_LEFT;
                //$matrix[$i][$j]{pointer} = "left";
            }
        }
//       printf("[%d,%d] diagonal_score: %d, up_score: %d, left_score: %d, score_matrix:%d \n", i, j, diagonal_score, up_score, left_score, score_matrix[i][j]); 
        // set maximum score
        if (*(score_matrix + ARRAY_OFFSET(i, j, ncols) )  > *max_score) {
            *max_i     = GLOBAL_ROW_NUMBER((i),rank,nrows) ;
            *max_j     = j;
            *max_score = *(score_matrix + ARRAY_OFFSET(i, j, ncols) ) ;
        }
     }
   } 

}

int  main(int argc,char *argv[]) { 
  MPI_Init(&argc, &argv);  
  int rank, nprocs;  
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  char  alphabet[21] = "acdefghiklmnpqrstvwy"; 
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

  int score_matrix[(seq2_length+1)/2][(seq1_length+1)] ;  
  int direction_matrix[(seq2_length+1)/2][(seq1_length+1)] ;  

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

  if (rank ==0) { 
    for (j=0; j <= seq1_length; j++) { 
      score_matrix[0][j] = 0; 
      direction_matrix[0][j]   = DIRECTION_NONE;
    } 
  } 

  for (i=0; i < (seq2_length+1)/2; i++ ) { 
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
