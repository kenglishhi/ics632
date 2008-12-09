/* sw_sequential.c */

#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include <math.h>

#define GLOBAL_ROW(I,RANK,NPROCS,CHUNK_SIZE) ( ((RANK) / ((int) sqrt(NPROCS) )) * (CHUNK_SIZE) +(I)    )
#define GLOBAL_COLUMN(J,RANK,NPROCS,CHUNK_SIZE) ( ((RANK) % ((int) sqrt(NPROCS)))*(CHUNK_SIZE) + (J)     )
#define ARRAY_OFFSET(I,J,NROWS)  ((I)*(NROWS) + (J))
#define DEBUG   0
#define STRLEN  8
#define ITER    1
#define GAP    -1
#define MATCH   1
#define MISMATCH -1
#define SEQ1_LEN 8
#define DIRECTION_NONE     0
#define DIRECTION_UP       1
#define DIRECTION_LEFT     2
#define DIRECTION_DIAGONAL 3
#define SEQ_INDEX(I,J,ROW_SIZE) ( (J) + ((I) *(ROW_SIZE) ) )


void generate_random_array(int *arr, int size, int rand_max);
double get_time_diff(struct timeval *start, struct timeval *finish);
void print_score_matrix(int *matrix, int nrows, int ncols);
void calculate_chunk(int *seq1_arr, int *seq2_arr, int *score_matrix, int *direction_matrix, int *prev_row, int *prev_col, int ncols_chunk, int *max_score, int *max_i, int *max_j)  ;

void Bottom_Send(int *buffer, int length )  ;
void Right_Send(int *buffer, int length ) ;

void Top_Recv(int *buffer, int length ) ;
void Left_Recv(int *buffer, int length ) ;

int getRightDestination() ;
int getBottomDestination() ;

int getLeftSource() ;
int getTopSource() ;


int isTopRowChunk() ;
int isBottomRowChunk() ;
int isLeftColumnChunk() ;
int isRightColumnChunk() ;

void generate_random_array(int *arr, int size, int rand_max) {
  int i;
  int val;
  for (i =0 ; i < size; i++ ){
    val = rand()% rand_max;
    *(arr + i ) = val;
  }
}

void Right_Send(int *buffer, int length ) {
  int dest = getRightDestination()  ;
  if (DEBUG)
    printf("Dest = %d \n", dest);
  MPI_Send(buffer, length, MPI_INT,  dest, 0, MPI_COMM_WORLD);
}

void Bottom_Send(int *buffer, int length ) {
  int dest = getBottomDestination()  ;
  if (DEBUG)
    printf("Dest = %d \n", dest);

  MPI_Send(buffer, length, MPI_INT,  dest, 0, MPI_COMM_WORLD);
}

void Left_Recv(int *buffer, int length )  {
  int src = getLeftSource()  ;
  if (DEBUG)
    printf("Source = %d \n", src);
  MPI_Status status ;
  MPI_Recv(buffer, length, MPI_INT, src, 0, MPI_COMM_WORLD, &status);

}

int isTopRowChunk() {
  int rank, nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  int width  =  sqrt(nprocs);
  if ( rank < width)  {
    return 1 ;
  }  else {
    return 0 ;
  }
}
int isBottomRowChunk() { 
  int rank, nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  int width  =  sqrt(nprocs);
  if ( (rank + width ) >= nprocs ) {
    return 1;
  }  else {
    return 0 ;
  }
} 

int isLeftColumnChunk() {
  int rank, nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  int width  =  sqrt(nprocs);
  if ( rank % width == 0 )  {
    return 1 ;
  }  else {
    return 0 ;
  }
}
int isRightColumnChunk() {
  int rank, nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  int width  =  sqrt(nprocs);
  if ( rank % width == (width -1)  )  {
    return 1 ;
  }  else {
    return 0 ;
  }
}

// for testing only, inside this library we should use the macros
int global_row(int i, int chunk_size) {
  int rank, nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  return GLOBAL_ROW(i,rank,nprocs,chunk_size);
}

// for testing only, inside this library we should use the macros
int global_column(int j, int chunk_size) {
  int rank, nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  return GLOBAL_COLUMN(j,rank,nprocs,chunk_size) ;
}

void Top_Recv(int *buffer, int length )  {
  int src = getTopSource()  ;
  if (DEBUG)
    printf("Source = %d \n", src);
  MPI_Status status ;
  MPI_Recv(buffer, length, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
}

int getRightDestination() {
  int rank, nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  int width  =  sqrt(nprocs);
  if (rank%width == (width -1)) {
    return (rank - width) + 1  ;
  }  else {
    return (  rank + 1) ;
  }
  return 0;
}

int getBottomDestination() {
  int rank, nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  int width  =  sqrt(nprocs);
  if ( (rank + width ) >= nprocs ) {
    return rank % width ;
  }  else {
    return ( rank + width ) ;
  }
}

int getLeftSource() {
  int rank, nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  int width  =  sqrt(nprocs);
  if (rank%width == 0 ) {
    return (rank + width) - 1  ;
  }  else {
    return (rank - 1) ;
  }
}

int getTopSource() {
  int rank, nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  int width  =  sqrt(nprocs);
  if ( rank  < width ) {
    return nprocs  - width + rank ;
  }  else {
    return ( rank - width ) ;
  }
}



double get_time_diff(struct timeval *start, struct timeval *finish) {
  // Copied from Casanova
  return (((finish->tv_sec*1000000.0+finish->tv_usec) - (start->tv_sec*1000000.0+start->tv_usec)) / 1000000.00);
}

void print_score_matrix(int *matrix, int nrows, int ncols) {
  int rank, nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  int *current_value;
  int i,j;

  for (i = 0; i < nrows; i++) {
    printf(" [" );
    for (j = 0; j < ncols; j++){
      current_value = matrix + (i * ncols) + j;
      printf("RANK%d, (%d,%d)=%d ", rank, GLOBAL_ROW(i, rank, nprocs, nrows), GLOBAL_COLUMN(j, rank, nprocs, ncols), *current_value  );
    }
    printf("] " );

    printf("\n" );
  }
}


void calculate_chunk(int *seq1_arr, int *seq2_arr, int *score_matrix, int *direction_matrix, int *prev_row, int *prev_col, int ncols_chunk, int *max_score, int *max_i, int *max_j) {
  int rank, nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
//   printf("Calculating chunk= %d, col_start\n", col_start);
  int row_start=0, col_start=0, row_end=ncols_chunk, col_end=ncols_chunk, i, j ;
  int letter1, letter2 ;
  int diagonal_score, left_score, up_score;
  int *diagonal_value_ptr, *left_value_ptr, *up_value_ptr;
  char alphabet[21] = "acdefghiklmnpqrstvwy";

  if (GLOBAL_ROW(0,rank,nprocs,ncols_chunk)  == 0 ) {
    row_start=1;
    printf("RANK%d Is first row \n", rank);
    for (i = 0; i < row_end+1;  i++ ) {
      prev_row[i] = 0 ;
    }
  }
  int last_prev_row_value = *(prev_row+row_end-1); 
  int last_prev_col_value = *(prev_col+col_end-1); 
  if (GLOBAL_COLUMN(0,rank,nprocs,ncols_chunk) == 0 ) {
    col_start=1;
    printf("RANK%d Is first column \n", rank);
    for (i = 0; i < col_end+1;  i++ ) {
      prev_col[i] = 0 ;
    }
  }

  if (prev_col[0] != prev_row[0] ) {
    printf("Assertion: prev_col[0] != prev_row[0] \n");
  }


  for (i = row_start; i < row_end;  i++ ) {
    for (j = col_start; j < col_end; j++ ) {
      // WRONG!
      if ( i==0 ) {
	up_value_ptr = (prev_row + j+1)  ;
        if (DEBUG) 
        printf(" RANK%d,  up_value_ptr = %d \n ", rank, *up_value_ptr );
      } else {
	up_value_ptr = (score_matrix + (i-1) * ncols_chunk + j );
      }

      if (j==0) {
	left_value_ptr = (prev_col + i + 1);
        if (DEBUG ) 
        printf(" RANK%d,j=%d  left_value_ptr = %d \n ", rank, j, *left_value_ptr );
        if (rank == 4) { 
          printf(" RANK%d,j=%d,i=%d, up_value_ptr = %d,left_value_ptr = %d \n ", rank, j,i, *up_value_ptr, *left_value_ptr );
        } 
      } else {
	left_value_ptr = (score_matrix + i * ncols_chunk + (j-1) );
      }


      if (j==0 && i==0) {
	diagonal_value_ptr = (prev_col) ;  // or prev_row, they should have the same value, see assertion above.
        if (rank ==4 ) 
        printf(" RANK%d,  diagonal_value_ptr = %d \n ", rank, *diagonal_value_ptr );
      } else if (i==0) {
	diagonal_value_ptr = (prev_row + j);
        if (DEBUG) 
        printf(" RANK%d,  diagonal_value_ptr = %d \n ", rank, *diagonal_value_ptr );
      } else if (j==0) {
	diagonal_value_ptr = (prev_col + i);
        if (DEBUG) 
        printf(" RANK%d,  diagonal_value_ptr = %d \n ", rank, *diagonal_value_ptr );
      } else {
	diagonal_value_ptr = (score_matrix + (i-1) * (ncols_chunk + (j-1) ) );
      }

      diagonal_score=0; left_score=0; up_score=0;

      letter1 = *(seq1_arr + GLOBAL_ROW((i-1), rank, nprocs, ncols_chunk) ) ;
      letter2 = *(seq2_arr + GLOBAL_COLUMN(j-1, rank, nprocs, ncols_chunk ) );

//      printf(" RANK%d, score[%d,%d] letter1 = %c letter2 = %c\n ", rank, GLOBAL_ROW(i, rank, nprocs, ncols_chunk), GLOBAL_COLUMN(j, rank, nprocs, ncols_chunk),  alphabet[letter1], alphabet[letter2]);

      if (DEBUG) 
      printf(" RANK%d, diagonal_value_ptr = %d\n ", rank, *diagonal_value_ptr );

      if (letter1 == letter2)  {
	diagonal_score = *diagonal_value_ptr + MATCH;
      }
      else {
	diagonal_score = *diagonal_value_ptr + MISMATCH;
      }
      up_score   = *up_value_ptr + GAP;
      left_score = *left_value_ptr + GAP;
      if (DEBUG)  { 
        printf(" RANK%d, diagonal_score = %d, left_score = %d, up_score = %d \n ", rank, diagonal_score, left_score, up_score );
        printf(" RANK%d, i= %d, j = %d, seq_index = %d \n ", rank, i, j, SEQ_INDEX(i,j,ncols_chunk)  );
      } 


      if ((diagonal_score <= 0) && (up_score <= 0) && (left_score <= 0)) {
	score_matrix[  SEQ_INDEX(i,j,ncols_chunk) ]   = 0;
	direction_matrix[ SEQ_INDEX(i,j,ncols_chunk) ]   = DIRECTION_NONE;

	if (i ==  (row_end- 1) ) {
	  prev_row[j+1] = 0 ;
	}
	if (j ==  (col_end -1) ) {
	  prev_col[i+1] = 0 ;
	}
	continue ;
      }

      // choose best score
      if (diagonal_score >= up_score) {
//	printf(" RANK%d, score_matrix= %d, direction_matrix = %d\n ", rank, score_matrix[ SEQ_INDEX(i,j,ncols_chunk)], direction_matrix[SEQ_INDEX(i,j,ncols_chunk)]   );
	if (diagonal_score >= left_score) {
	  score_matrix[ SEQ_INDEX(i,j,ncols_chunk) ]   = diagonal_score;
	  direction_matrix[ SEQ_INDEX(i,j,ncols_chunk) ]   = DIRECTION_DIAGONAL;
	}
	else {
	  score_matrix[ SEQ_INDEX(i,j,ncols_chunk) ] = left_score;
	  direction_matrix[ SEQ_INDEX(i,j,ncols_chunk) ]   = DIRECTION_LEFT;
	}
      } else {
	if (up_score >= left_score) {
	  score_matrix[ SEQ_INDEX(i,j,ncols_chunk) ]  = up_score;
	  direction_matrix[SEQ_INDEX(i,j,ncols_chunk) ]   = DIRECTION_UP;
	}
	else {
	  score_matrix[ SEQ_INDEX(i,j,ncols_chunk) ] = left_score;
	  direction_matrix[SEQ_INDEX(i,j,ncols_chunk) ]   = DIRECTION_LEFT;
	}
      }

      if (score_matrix[SEQ_INDEX(i,j,ncols_chunk)] > *max_score) {
	*max_i     = i;
	*max_j     = j;
	*max_score = score_matrix[ SEQ_INDEX(i,j,ncols_chunk) ] ;
      }
      if (i ==  (row_end -1) ) {
	prev_row[j+1] = score_matrix[ SEQ_INDEX(i,j,ncols_chunk) ] ;
      }
      if (j ==  (col_end -1) ) {
	prev_col[i+1] = score_matrix[ SEQ_INDEX(i,j,ncols_chunk) ]  ;
      }
    }
  }
  
  prev_row[0] = last_prev_row_value; 
  prev_col[0] = last_prev_col_value; 

  return ;
}
void backtrace_direciton_matrix(int *seq1_arr, int *seq2_arr, int *direction_matrix, int *top_i, int *top_j,int ncols,int nrows, int *output1_arr, int *output2_arr,int *align1_index,int *align2_index, int *completion_flag ) {

  int rank, nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  char *program_name;
  char alphabet[21] = "acdefghiklmnpqrstvwy";
  program_name = "sw_ring_random_sync";
  int *diagonal_ptr;
  int i, j;
  i = *top_i; j = *top_j ;
  printf("%s, RANK%d, completion_flag = %d !!!!!\n", program_name, rank, *completion_flag ) ;
  int done_flag = 1 ;
  while (1) {
    diagonal_ptr = (direction_matrix + ARRAY_OFFSET(i, j, ncols)) ;
    if (*diagonal_ptr == DIRECTION_NONE  ) {
      if (DEBUG)
	printf("%s, RANK%d, No Direction!!!!!\n", program_name, rank  ) ;
      completion_flag = &done_flag;
      break ;
    }
    if (i < 0  ) {
      if (DEBUG)
	printf("%s, RANK%d, i < 0 %d !!!!!\n", program_name, rank,i   ) ;
      completion_flag = &done_flag;
      break ;
    }
    if (GLOBAL_ROW(i,rank,nprocs, nrows) == 0 ) {
      if (DEBUG)
	printf("%s, RANK%d, i == 0 !!!!!\n", program_name, rank  ) ;
      completion_flag = &done_flag;
      break ;
    }


    if (*diagonal_ptr ==  DIRECTION_DIAGONAL ) {
      if (DEBUG)
	printf("%s, RANK%d, [%d,%d] DIRECTION_DIAGONAL !!!!!\n", program_name, rank, GLOBAL_ROW(i,rank,nprocs, nrows), j   ) ;
      output1_arr[*align1_index] = seq1_arr[j-1]  ;
      output2_arr[*align2_index] = seq2_arr[i-1]  ;
      if (DEBUG)
	printf("%s, RANK%d, [%d,%d] output1_arr = %c !!!!!\n", program_name, rank, GLOBAL_ROW(i,rank,nprocs, nrows), j,alphabet[output1_arr[*align1_index]]    ) ;
      i--; j--;
    }
    else if (*diagonal_ptr ==  DIRECTION_LEFT  ) {
      if (DEBUG)
	printf("%s, RANK%d, DIRECTION_LEFT !!!!!\n", program_name, rank  ) ;
      output1_arr[*align1_index] = seq1_arr[j-1]  ;
      output2_arr[*align2_index] = -1 ;
      j--;
    }
    else if (*diagonal_ptr  ==  DIRECTION_UP ) {
      if (DEBUG)
	printf("%s, RANK%d, DIRECTION_UP !!!!!\n", program_name, rank  ) ;
      output1_arr[*align1_index] = -1;
      output2_arr[*align2_index] = seq2_arr[i-1]  ;
      i--;
    }
    *align1_index= *align1_index+1 ; *align2_index = *align2_index+1 ;
  }
  *top_i =i ;  *top_j = j  ;
}
