/*************************************
 *
 * sw_seq_lib.c
 * Author: Kevin English
 * University of Hawaii
 * ICS 632
 *
 * This is library of functions used for our sequential implementation of Smith-Waterman
 **/

#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>


#define DEBUG   0
#define STRLEN     8
#define   ITER       1
#define GAP   -1
#define MATCH 1
#define MISMATCH -1
#define SEQ1_LEN 8
#define DIRECTION_NONE     0
#define DIRECTION_UP       1
#define DIRECTION_LEFT     2
#define DIRECTION_DIAGONAL 3
#define SEQ_INDEX(I,J,ROW_SIZE) ( (J) + ((I) *(ROW_SIZE) ) )

void generate_random_array(int *arr, int size, int rand_max, int seed) {
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
  int *current_value;
  int i,j;

  printf("nrows , col_size = [%d,%d]  \n\n", nrows, ncols );
  for (i = 0; i < nrows; i++) {
    printf(" [" );
    for (j = 0; j < ncols; j++){
      current_value = matrix + (i * ncols) + j;
      printf("(%d,%d)=%d ", i, j, *current_value  );
    }
    printf("] " );

    printf("\n" );
  }
}


int do_sw(int *seq1_arr, int seq1_length, int *seq2_arr, int seq2_length, int *output1_arr, int *output1_length, int *output2_arr, int *output2_length ) {

  if (DEBUG) {
    printf("seq1_length = %d\n", seq1_length );
    printf("seq2_length = %d\n", seq2_length );
  }

//  char *seq1 = "ppeaiccc";
//  char *seq2 = "ggeaicgg";

  int i, j;
  int *score_matrix, *direction_matrix;
  score_matrix = (int *) malloc((seq1_length+1)*(seq1_length+1) * sizeof(int) )  ;
  direction_matrix = (int *) malloc((seq1_length+1)*(seq1_length+1) * sizeof(int) )  ;
//  int score_matrix[seq2_length+1][seq1_length+1] ;
//  int direction_matrix[seq2_length+1][seq1_length+1] ;

  int max_i=-1, max_j=-1, max_score=-1 ;
  int diagonal_score, left_score, up_score  ;
  int letter1, letter2;
  int matrix_width = seq1_length+1 ;

  /* Convert residue characters to indices */
  if (DEBUG) {
    printf("STRLEN = %d\n", STRLEN);
    printf("d_loc = %d\n", STRLEN);
  }
  for (j=0; j <= seq1_length; j++) {
    score_matrix[SEQ_INDEX(0,j,matrix_width)] = 0;
    direction_matrix[SEQ_INDEX(0,j,matrix_width)]   = DIRECTION_NONE;
  }
  printf("rows = seq1_length  = %d\n", seq1_length);
  for (i=0; i <= seq2_length; i++ ) {
    score_matrix[SEQ_INDEX(i,0,matrix_width) ] = 0;
    direction_matrix[SEQ_INDEX(i,0,matrix_width) ]   = DIRECTION_NONE;
  }
  printf("cols = seq2_length  = %d\n", seq2_length);

  struct timeval start;
  struct timeval finish;

  gettimeofday(&start,NULL);

  for (i=1; i <= seq2_length; i++) {
    for (j=1; j <= seq1_length; j++) {

      diagonal_score=0; left_score=0; up_score=0;
      letter1 = seq1_arr[i-1];
      letter2 = seq2_arr[j-1];
      // calculate match score

      if (letter1 == letter2)  {
	diagonal_score = score_matrix[SEQ_INDEX(i-1,j-1,matrix_width) ] + MATCH;
      }
      else {
	diagonal_score = score_matrix[ SEQ_INDEX(i-1, j-1, matrix_width) ] + MISMATCH;
      }

      // calculate gap scores
      up_score   = score_matrix[ SEQ_INDEX(i-1,j,matrix_width) ] + GAP;
      left_score = score_matrix[ SEQ_INDEX(i,j-1,matrix_width) ] + GAP;

//       if (i==1 && j==1) {
//          printf("[%d,%d] diagonal_score: %d, up_score: %d, left_score: %d, letter1=%d, letter2=%d \n", i, j, diagonal_score, up_score, left_score, letter1, letter2 );
//            printf("%d [%d,%d] diagonal_score: %d, up_score: %d, left_score: %d,letter1=%d, letter2=%d \n", ARRAY_OFFSET(i,j,ncols), i, j, diagonal_score,
//                up_score, left_score, letter1, letter2 );
//      }
      if ((diagonal_score <= 0) && (up_score <= 0) && (left_score <= 0)) {
	score_matrix[  SEQ_INDEX(i,j,matrix_width) ]   = 0;
//            printf("[%d,%d] diagonal_score: %d, up_score: %d, left_score: %d, score_matrix:%d \n", i, j, diagonal_score, up_score, left_score, score_matrix[i][j]);
	direction_matrix[ SEQ_INDEX(i,j,matrix_width) ]   = DIRECTION_NONE;
	continue;
      }

      // choose best score
      if (diagonal_score >= up_score) {
	if (diagonal_score >= left_score) {
	  score_matrix[ SEQ_INDEX(i,j,matrix_width) ]   = diagonal_score;
	  direction_matrix[ SEQ_INDEX(i,j,matrix_width) ]   = DIRECTION_DIAGONAL;
	}
	else {
	  score_matrix[ SEQ_INDEX(i,j,matrix_width) ] = left_score;
	  direction_matrix[ SEQ_INDEX(i,j,matrix_width) ]   = DIRECTION_LEFT;
	}
      } else {
	if (up_score >= left_score) {
	  score_matrix[ SEQ_INDEX(i,j,matrix_width) ]  = up_score;
	  direction_matrix[SEQ_INDEX(i,j,matrix_width) ]   = DIRECTION_UP;
	}
	else {
	  score_matrix[ SEQ_INDEX(i,j,matrix_width) ] = left_score;
	  direction_matrix[SEQ_INDEX(i,j,matrix_width) ]   = DIRECTION_LEFT;
	}
      }

//       printf("[%d,%d] diagonal_score: %d, up_score: %d, left_score: %d, score_matrix:%d \n", i, j, diagonal_score, up_score, left_score, score_matrix[i][j]);

      // set maximum score
      if (score_matrix[SEQ_INDEX(i,j,matrix_width)] > max_score) {
	max_i     = i;
	max_j     = j;
	max_score = score_matrix[ SEQ_INDEX(i,j,matrix_width) ] ;
      }
    }
  }

  gettimeofday(&finish,NULL);
  printf("%.8f Scoring the sequence \n", get_time_diff(&start, &finish) *1000000.0) ;


  if (DEBUG) {
    printf("max_i = %d\n", max_i);
    printf("max_j = %d\n", max_j);
    printf("max_score = %d\n", max_score);
  }
//  for (i=0; i <= seq2_length; i++) {
//    for (j=0; j <= seq1_length; j++) {
//      printf("score_matrix[%d][%d]  %d\n", i,j, score_matrix[SEQ_INDEX(i,j,matrix_width)] );
//    }
//  }


  if (DEBUG) {
    printf("SCORE MATRIX:\n");
//  printf("DIRECTION MATRIX:\n");
//  print_score_matrix(direction_matrix,  seq2_length+1, matrix_width  );
    printf("finished printing score matrix= %d\n", max_score);
    printf("max_i = %d, max_j = %d\n", max_i, max_j);
    print_score_matrix(score_matrix,  seq2_length+1, matrix_width  );
  }
//  trace-back

// char align1[SEQ1_LEN] = "ppeaiccc";
// char align2[SEQ1_LEN] = "ppeaiccc";
// int output1_arr[seq1_length], output2_arr[seq2_length]  ;

//align1 = "";
//align2 = "";
//
  i = max_i;
  j = max_j;

  int align1_index=0;
  int align2_index=0;

  gettimeofday(&start,NULL);
  while (1) {
    if (direction_matrix[SEQ_INDEX(i,j,matrix_width) ] == DIRECTION_NONE)
      break ;

    if (direction_matrix[SEQ_INDEX(i,j,matrix_width)] ==  DIRECTION_DIAGONAL ) {
      output1_arr[align1_index] = seq1_arr[j-1]  ;
      output2_arr[align2_index] = seq2_arr[i-1]  ;
      i--; j--;
    }
    else if (direction_matrix[SEQ_INDEX(i,j,matrix_width)] ==  DIRECTION_LEFT  ) {
      output1_arr[align1_index] = seq1_arr[j-1]  ;
      output2_arr[align2_index] = -1 ;
      j--;
    }
    else if (direction_matrix[SEQ_INDEX(i,j,matrix_width)] ==  DIRECTION_UP ) {
      output1_arr[align1_index] = -1;
      output2_arr[align2_index] = seq2_arr[i-1]  ;
      i--;
    }
    align1_index++; align2_index++;
  }
  printf("align1_index = %d, align2_index = %d\n", align1_index, align2_index);
  gettimeofday(&finish,NULL);

  printf("%f Doing Back Trace of the sequence \n", get_time_diff(&start, &finish) ) ;

  if (DEBUG)
    printf("align1_index = %d, align2_index = %d\n", align1_index, align2_index);

  *output1_length = align1_index;
  *output2_length = align2_index;


  return 1;
}

