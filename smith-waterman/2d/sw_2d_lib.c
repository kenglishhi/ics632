/* sw_sequential.c */

#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include <math.h> 

#define GLOBAL_ROW_NUMBER(I,RANK,ROW_SIZE) ( (RANK)*(ROW_SIZE)+(I) )
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

void generate_random_array(int *arr, int size, int rand_max); 
double get_time_diff(struct timeval *start, struct timeval *finish); 
void print_score_matrix(int *matrix, int nrows, int ncols); 
void calculate_chunk(int *seq1_arr, int *seq2_arr, int *score_matrix, int *direction_matrix, int *prev_row, int *prev_col, int width)  ;

void Bottom_Send(int *buffer, int length )  ; 
void Right_Send(int *buffer, int length ) ; 

void Top_Recv(int *buffer, int length ) ; 
void Left_Recv(int *buffer, int length ) ; 

int getRightDestination() ;
int getBottomDestination() ;

int getLeftSource() ;
int getTopSource() ;


int isTopRowChunk() ; 
int isLeftColumnChunk() ; 

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
       return  1 ;
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
       return  1 ;
    }  else {
       return 0 ;
    }
}

int global_row(int i, int chunk_size) {
    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    int width  =  sqrt(nprocs); 
//    int row = rank/width ; 
    return ((rank) / (width)) * (chunk_size) +(i) ;   
} 

int global_column(int j, int chunk_size) {
    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    int width  =  sqrt(nprocs); 
//    int column = rank%width ; 
    return ((rank) % (width))*(chunk_size) + (j) ;   
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
       return  (rank - width) + 1  ;
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
       return  rank % width ;
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
       return  (rank + width) - 1  ;
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
   int rank;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   int *current_value;
   int i,j;

   if (DEBUG) 
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


void calculate_chunk(int *seq1_arr, int *seq2_arr, int *score_matrix, int *direction_matrix, int *prev_row, int *prev_col, int ncols) {
   int rank, nprocs;  
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
//   printf("Calculating chunk= %d, col_start\n", col_start);  


   return ; 
}
void backtrace_direciton_matrix(int *seq1_arr, int *seq2_arr, int *direction_matrix, int *top_i, int *top_j,int ncols,int nrows, int *output1_arr, int *output2_arr,int *align1_index,int *align2_index, int *completion_flag ) {
  int rank;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  char *program_name;
  char  alphabet[21] = "acdefghiklmnpqrstvwy";
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
              printf("%s, RANK%d, i < 0 %d !!!!!\n", program_name, rank ,i   ) ;
            completion_flag = &done_flag; 
            break ;
        }
        if (GLOBAL_ROW_NUMBER(i,rank,nrows) == 0 ) {
            if (DEBUG)  
              printf("%s, RANK%d, i == 0 !!!!!\n", program_name, rank  ) ;
            completion_flag = &done_flag; 
            break ;
        }
        

        if (*diagonal_ptr ==  DIRECTION_DIAGONAL ) {
            if (DEBUG)  
              printf("%s, RANK%d, [%d,%d] DIRECTION_DIAGONAL !!!!!\n", program_name, rank, GLOBAL_ROW_NUMBER(i,rank,nrows), j   ) ;
            output1_arr[*align1_index] = seq1_arr[j-1]  ;
            output2_arr[*align2_index] = seq2_arr[i-1]  ;
            if (DEBUG)  
              printf("%s, RANK%d, [%d,%d] output1_arr = %c !!!!!\n", program_name, rank, GLOBAL_ROW_NUMBER(i,rank,nrows), j,alphabet[output1_arr[*align1_index]]    ) ;
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
