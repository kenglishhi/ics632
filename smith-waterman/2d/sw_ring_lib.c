/* sw_sequential.c */

#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

#define GLOBAL_ROW_NUMBER(I,RANK,ROW_SIZE) ( (RANK)*(ROW_SIZE)+(I) )
#define ARRAY_OFFSET(I,J,NROWS)  ((I)*(NROWS) + (J))
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

void generate_random_array(int *arr, int size, int rand_max) { 
   int i; 
   int val; 

   for (i =0 ; i < size; i++ ){ 
      val = rand()% rand_max;
      *(arr + i ) = val;
   } 
} 

double get_time_diff(struct timeval *start, struct timeval *finish) {
    // Copied from Casanova
    return (((finish->tv_sec*1000000.0+finish->tv_usec) - (start->tv_sec*1000000.0+start->tv_usec)) / 1000000.00);
}

void print_score_matrix(int *matrix, int nrows, int ncols) {
   int rank;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   int *current_value;
   int i,j;

   printf("nrows , col_size = [%d,%d]  \n\n", nrows, ncols );
   for (i = 0; i < nrows; i++) {
        printf(" [" );
        for (j = 0; j < ncols; j++){
            current_value = matrix + (i * ncols) + j;
            printf("(%d,%d)=%d ", GLOBAL_ROW_NUMBER(i,rank,nrows), j, *current_value  );
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
   int diagonal_score, left_score, up_score;
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
