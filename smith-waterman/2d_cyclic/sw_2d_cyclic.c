/* sw_paralle.c */

#include  <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <mpi.h>
#include <math.h>


#define DEBUG    1
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
#define CHUNK_OFFSET(NROWS, NCOLS, CHUNK, CHUNK_SIZE) (  (((NROWS)-1)*(NCOLS) ) +  (CHUNK)*(CHUNK_SIZE) )
#define GLOBAL_ROW_NUMBER(I,RANK,ROW_SIZE) ( (RANK)*(ROW_SIZE)+(I) )
#define LOCAL_ROW_NUMBER(GLOBAL_ROW_NUM,RANK,ROW_SIZE) ( GLOBAL_ROW_NUM - (RANK)*(ROW_SIZE) )
#define ARRAY_OFFSET(I,J,NROWS)  ((I)*(NROWS) + (J))

void generate_random_array(int *arr, int size, int rand_max);
double get_time_diff(struct timeval *start, struct timeval *finish);
void print_score_matrix(int *matrix, int nrows, int ncols);
void calculate_chunk(int *seq1_arr, int *seq2_arr, int *score_matrix, int *direction_matrix, int *prev_row, int *prev_col, int *new_prev_row, int *new_prev_col, int ncols_chunk, int *max_score, int *max_i, int *max_j) ;


int get_right_destination() ;
int get_bottom_destination() ;

void Bottom_Send(int *buffer, int length )  ;
void Right_Send(int *buffer, int length ) ;

void Top_Recv(int *buffer, int length ) ;
void Left_Recv(int *buffer, int length ) ;


int isTopRowChunk() ;
int isBottomRowChunk() ;

int isLeftColumnChunk() ;
int isRightColumnChunk() ;


int  main(int argc,char *argv[]) {
  /* Initialize MPI */
  MPI_Init(&argc, &argv);
  int rank, nprocs ;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  char *program_name = "sw_2d" ;

  if (nprocs != 4  && nprocs != 9  ) {
    printf("[%s] Current program limitation, nprocs must be 4 or 9\n", program_name );
    return 1;
  }

  struct timeval total_start;
  struct timeval total_finish;

  gettimeofday(&total_start,NULL);

  char alphabet[21] = "acdefghiklmnpqrstvwy";
  int ncols_matrix, ncols_chunk, seq1_length, seq2_length,ncycles;
  int i,j;
  int *seq1_arr, *seq2_arr, *align1_arr, *align2_arr;
  int *score_matrix, *direction_matrix, *prev_row, *prev_col , *new_prev_row,*new_prev_col  ;
  char *seq1, *seq2;

  int max_i=-1, max_j=-1, max_score=-1;

/*
  if (argc < 3) {
    printf("[%s] You need 2 arguments\n", program_name);
    return 0 ;
  } else {
    if ((sscanf(argv[1],"%d",&ncols_matrix) != 1) || (sscanf(argv[2],"%d",&ncycles) != 1) )  {
      fprintf(stderr,"Usage: %s <ncols> <number of cycles>\n", argv[0] );
      exit(1);
    }
  }
*/
  ncols_matrix = 8 ; 
  ncols_chunk = 2 ; 
  ncycles = 4; 
  

  if (ncols_matrix%nprocs != 0 ) {
    printf("[%s] Number of Columns (%d) must be divisible by number of procs (%d)\n", program_name, ncols_matrix, nprocs );
  }

//  ncols_chunk = ncols_matrix / ((int) sqrt(nprocs) * ncycles ) ;
  seq1_length = ncols_matrix - 1;
  seq2_length = seq1_length ;

/*  if (DEBUG) {
    printf("[%s] ncols_matrix: %d, nprocs:%d, ncols_chunk: %d\n",program_name, ncols_matrix, nprocs,    ncols_chunk) ;
    printf("[%s] seq1_length: %d, seq2_length:%d, \n",program_name, seq1_length, seq2_length) ;
  }
*/

  seq1_arr = (int *) malloc(seq1_length * sizeof(int) )  ;
  seq2_arr = (int *) malloc(seq2_length * sizeof(int) )  ;
  align1_arr = (int *) malloc(seq1_length * sizeof(int) )  ;
  align2_arr = (int *) malloc(seq2_length * sizeof(int) )  ;

  if (rank == 0 ) {
    srand(time(0)) ;
//    generate_random_array(seq1_arr, seq1_length,  20);

    seq1_arr[0] = 13; 
    seq1_arr[1] = 6;
    seq1_arr[2] = 18; 
    seq1_arr[3] = 4; 
    seq1_arr[4] = 11;
    seq1_arr[5] = 0; 
    seq1_arr[6] = 16;
//    if (argc == 4)
      seq2_arr = seq1_arr;
//    else
//      generate_random_array(seq2_arr, seq1_length,  20);

    if (DEBUG) {

      seq1 = (char *) malloc(seq1_length * sizeof(char) )  ;
      seq2 = (char *) malloc(seq2_length * sizeof(char) )  ;

      for(i=0; i < seq1_length; i++){
	seq1[i] =  alphabet[seq1_arr[i]] ;
      }

      for(i=0; i < seq2_length; i++){
	seq2[i] =  alphabet[seq2_arr[i]] ;
      }

      printf("[%s] seq1 = %s --\n", program_name, seq1);
      printf("[%s] seq2 = %s --\n", program_name, seq2);
    }
  }

  if (MPI_Bcast(seq1_arr, seq1_length, MPI_INT, ROOT, MPI_COMM_WORLD) ) {
    printf("Error while calling MPI_Bcast()\n");
    exit(0);
  }

  if (MPI_Bcast(seq2_arr, seq2_length, MPI_INT, ROOT, MPI_COMM_WORLD) ) {
    printf("Error while calling MPI_Bcast()\n");
    exit(0);
  }

  int matrix_of_matrix[ncycles]; 
  int global_i, global_j;   
  for (i=0 ; i < ncycles/2; i++) { 
    global_i = i + ncycles/2 ; 
    for (j=0 ; j < ncycles/2; j++) { 
      global_j = j + ncycles/2; 
      printf("RANK%d, section [%d, %d] \n", rank, global_i, global_j ); 
    } 
  } 
  return 0; 
  score_matrix = (int *) calloc(ncols_chunk  * ncols_chunk, sizeof(int) )  ;
  direction_matrix = (int *) calloc(ncols_chunk  * ncols_chunk, sizeof(int) )  ;

  prev_row = (int *) calloc(ncols_chunk+1, sizeof(int) )  ;
  prev_col = (int *) calloc(ncols_chunk+1, sizeof(int) )  ;

  new_prev_row = (int *) calloc(ncols_chunk+1, sizeof(int) )  ;
  new_prev_col = (int *) calloc(ncols_chunk+1, sizeof(int) )  ;


  for (i =0; i < ncols_chunk+1; i ++ ) {
    prev_row[i] = 999;
  } 
  for (i =0; i < ncols_chunk+1; i ++ ) {
    prev_col[i] = 888;
  } 

  if (isTopRowChunk() ) {
    for (j=0; j < ncols_chunk ; j++) {
      score_matrix[j] = 0;
      direction_matrix[j]   = DIRECTION_NONE;
    }
  }

  if (isLeftColumnChunk() ) {
    for (i=0; i < ncols_chunk; i++ ) {
      score_matrix[i * ncols_chunk ] = 0;
      direction_matrix[i * ncols_chunk ]   = DIRECTION_NONE;
    }
  }

  if (!isTopRowChunk() ) { 
    Top_Recv(prev_row, ncols_chunk+1 ) ; 
  } 

  if (!isLeftColumnChunk() ) { 
    Left_Recv(prev_col, ncols_chunk+1 ) ; 
  } 

  if (0 ) { 
//  if ( rank == 0 || rank == 3 || rank == 4 || rank == 1) { 

    printf("RANK%d PRE prev_row[%d] =%d", rank, j, prev_row[j] ); 
    for (j=0; j<=ncols_chunk ; j++ ) { 
      printf("%d, ", prev_row[j] ); 
    } 
    printf("\n");

    printf("RANK%d PRE prev_col[%d] =%d", rank, j, prev_col[j] ); 
    for (j=0; j<=ncols_chunk ; j++ ) { 
      printf("%d, ", prev_col[j] ); 
    } 
    printf("\n");
  } 

  calculate_chunk(seq1_arr, seq1_arr, score_matrix, direction_matrix, prev_row, prev_col, new_prev_row, new_prev_col, ncols_chunk, &max_score, &max_i, &max_j ) ;
//  if (rank == 3 || rank==0 || rank ==1 || rank == 4 ) { 
//  if (rank == 0 || rank==1  || rank == 4  || rank == 1) { 
//    print_score_matrix(score_matrix, ncols_chunk, ncols_chunk);
//  } 
  if (0 ) { 
//  if (rank && 0 || rank == 3 || rank == 4 || rank == 1) { 

    printf("RANK%d POST prev_row[%d] =%d", rank, j, prev_row[j] ); 
    for (j=0; j<=ncols_chunk ; j++ ) { 
      printf("%d, ", prev_row[j] ); 
    } 
    printf("\n");

    printf("RANK%d POST prev_col[%d] =%d", rank, j, prev_col[j] ); 
    for (j=0; j<=ncols_chunk ; j++ ) { 
      printf("%d, ", prev_col[j] ); 
    } 
    printf("\n");
  } 


 
  if (!isBottomRowChunk() ) { 
    Bottom_Send(prev_row, ncols_chunk+1 ) ;
  } 
  if (!isRightColumnChunk() ) { 
    Right_Send(prev_col, ncols_chunk+1 ) ;
  } 
 
    print_score_matrix(score_matrix, ncols_chunk, ncols_chunk);
/*
    Bottom_Send(prev_row, ncols_chunk+1 ) ;
    Right_Send(prev_col, ncols_chunk+1 ) ;
    print_score_matrix(score_matrix, ncols_chunk, ncols_chunk);
  } else if (
*/

/*
  //if ((rank == 0 ) || (rank ==1 ) )  {
  if ((rank == 0 ) )  {
    calculate_chunk(seq1_arr, seq1_arr, score_matrix, direction_matrix, prev_row, prev_col, ncols_chunk, &max_score, &max_i, &max_j ) ;
    Bottom_Send(prev_row, ncols_chunk+1 ) ;
    Right_Send(prev_col, ncols_chunk+1 ) ;
//    for (j=0; j<=ncols_chunk ; j++ ) { 
////      printf("RANK%d prev_col[%d] = %d\n", rank, j, prev_col[j] ); 
//    } 
  } else if(rank == 1) {
   Left_Recv(prev_col, ncols_chunk+1 ) ; 
//   for (j=0; j<=ncols_chunk ; j++ ) { 
//      printf("RANK%d prev_col[%d] = %d\n", rank, j, prev_col[j] ); 
//    } 
    calculate_chunk(seq1_arr, seq1_arr, score_matrix, direction_matrix, prev_row, prev_col, ncols_chunk, &max_score, &max_i, &max_j ) ;
  } else if(rank == 3) {
    Top_Recv(prev_row, ncols_chunk+1 ) ; 
//    for (i=0; i<=ncols_chunk ; i++ ) { 
//      printf("RANK%d prev_row[%d] = %d\n", rank, i, prev_row[i] ); 
//    }
    calculate_chunk(seq1_arr, seq1_arr, score_matrix, direction_matrix, prev_row, prev_col, ncols_chunk, &max_score, &max_i, &max_j ) ;
  } 
*/
// if ((rank ==0)  || (rank == 1) || (rank == 3) ) { 
//    print_score_matrix(score_matrix, ncols_chunk, ncols_chunk);
/*
*/
//  } 
  gettimeofday(&total_finish,NULL);
//  if (DEBUG) {
    printf("%s, Time, Columns, Chunk, nprocs,rank\n", program_name) ;
    printf("%s, %f, %d, %d, %d, %d, result \n", program_name, get_time_diff(&total_start, &total_finish), ncols_matrix, ncols_chunk,nprocs, rank ) ;
//  }

//  printf("%s, Time, Columns, Chunk, nprocs\n", program_name) ;
//  printf("%s, %f, %d, %d, %d, result \n", program_name, get_time_diff(&total_start, &total_finish), ncols, chunk_size,nprocs ) ;

  return 0 ;

}
