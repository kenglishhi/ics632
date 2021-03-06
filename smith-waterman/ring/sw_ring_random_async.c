/*************************************
 *
 * sw_ring_random_async.c
 * Author: Kevin English
 * University of Hawaii
 * ICS 632
 *
 * This program takes a length and a chunk size from that generates 2 random strings. 
 * It then runs the Smith-Waterman algorithm on the two strings  and determines the optimal local alignment.
 * An extra flag may be sent to program so that it generates one random string and then runs SW on itself.
 * This is useful to find the worst case of the backtrace algorithm.
 * This program uses blocking sends and receives.
 **/



#include  <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <mpi.h>

#define DEBUG    0
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

void Ring_Send(int *, int ) ;
void Ring_Recv(int *, int ) ;
void Ring_Isend(int *, int,  MPI_Request * ) ;
void Ring_Irecv(int *, int,  MPI_Request * ) ;
void Ring_Wait(MPI_Request *request) ;



void Ring_Reverse_Send(int *buffer, int length ) ;
void Ring_Reverse_Recv(int *buffer, int length ) ;

double get_time_diff(struct timeval *, struct timeval *);

void calculate_chunk(int *, int *, int *, int *, int *, int,int, int, int, int *, int *, int *) ;
void generate_random_array(int *, int, int);
void print_score_matrix(int *, int, int ) ;
void backtrace_direciton_matrix(int *seq1_arr, int *seq2_arr, int *direction_matrix, int *top_i, int *top_j, int ncols,int nrows, int *align1_arr, int *align2_arr,int *align1_length,int *align2_length, int *completion_flag ) ;

int  main(int argc,char *argv[]) {
  /* Initialize MPI */
  MPI_Init(&argc, &argv);
  int rank, nprocs ;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);


  struct timeval total_start;
  struct timeval total_finish;

  gettimeofday(&total_start,NULL);


  char alphabet[21] = "acdefghiklmnpqrstvwy";
  char *program_name;
  program_name = "sw_ring_random_async";
  /* Parse Command Line Args */
  int nrows, ncols, chunk_size ;
  if (argc < 3) {
    printf("[%s] You need 2 arguments\n", program_name);
    return 0 ;
  } else {
    if ((sscanf(argv[1],"%d",&ncols) != 1) ||
        (sscanf(argv[2],"%d",&chunk_size) != 1)) {
      fprintf(stderr,"Usage: %s <ncols> <chunk_size>\n", argv[0] );
      exit(1);
    }
  }
  int seq1_length = ncols - 1;
  int seq2_length = seq1_length ;

  /*  Check for current constraints of the program */

  nrows =  ncols/nprocs;
//  int ncols =  (seq1_length+1);


  if (DEBUG)   {
    printf("%s, RANK%d, nrows = %d, ncols = %d, chunk_size = %d, seq1_length = %d \n", program_name, rank, nrows, ncols, chunk_size, seq1_length);
    printf("%s, RANK%d, nrows = %d, ncols = %d\n", program_name, rank, nrows, ncols);
  }
  if (seq1_length != seq2_length) {
    printf("Current program limitation, seq1_length != seq2_length \n");
    return 1;
  }

  if (((ncols) % nprocs) != 0 ) {
    printf("Current program limitation, ncols (%d) must be divisible by nprocs (%d)  \n", (ncols), nprocs );
    return 1;
  }

//  if (((ncols) % 2) != 0 ) {
//    printf("Current program limitation, ncols (%d) must be divisible 2 (%d)  \n", (ncols), nprocs );
//    return 1;
//  }

//  int chunk_size = 10; // (ncols)/2;
  if (((ncols) % chunk_size) != 0 ) {
    printf("Current program limitation, ncols (%d) must be 2 (%d)  \n", (ncols), nprocs );
    return 1;
  }


  /*  Set the Chunk Length */


  srand(time(0)) ;
  int i,j;
  int *seq1_arr, *seq2_arr, *align1_arr, *align2_arr;
  // all processes will have a copy of Sequence 1
  // They will have a slice of sequence 2
  seq1_arr = (int *) malloc(seq1_length * sizeof(int) )  ;
  seq2_arr = (int *) malloc(seq2_length * sizeof(int) )  ;
  align1_arr = (int *) malloc(seq1_length * sizeof(int) )  ;
  align2_arr = (int *) malloc(seq2_length * sizeof(int) )  ;
  char *seq1, *seq2;

  if (rank == 0 ) {
    generate_random_array(seq1_arr, seq1_length,  20);
    if (argc == 4)
      seq2_arr = seq1_arr;
    else
      generate_random_array(seq2_arr, seq1_length,  20);

    seq1 = (char *) malloc(seq1_length * sizeof(char) )  ;
    seq2 = (char *) malloc(seq2_length * sizeof(char) )  ;

    if (DEBUG) {
      for(i=0; i < seq1_length; i++){
	seq1[i] =  alphabet[seq1_arr[i]] ;
      }

      for(i=0; i < seq2_length; i++){
	seq2[i] =  alphabet[seq2_arr[i]] ;
      }
      printf("seq1 = %s --\n", seq1);
      printf("seq2 = %s --\n", seq2);
    }
  }

  if (DEBUG)
    printf("%s,%d, seq2_length: %d, ncols: %d, nrows: %d \n", program_name, rank,  seq2_length, ncols, nrows   );
  if (MPI_Bcast(seq1_arr, seq1_length, MPI_INT, ROOT, MPI_COMM_WORLD) ) {
    printf("Error while calling MPI_Bcast()\n");
    exit(0);
  }

  if (MPI_Bcast(seq2_arr, seq2_length, MPI_INT, ROOT, MPI_COMM_WORLD) ) {
    printf("Error while calling MPI_Bcast()\n");
    exit(0);
  }

  int *score_matrix, *direction_matrix ;
  score_matrix = (int *) malloc(nrows  * ncols * sizeof(int) )  ;
  direction_matrix = (int *) malloc(nrows  * ncols * sizeof(int) )  ;


  if (rank ==0) {
    for (j=0; j <= seq1_length; j++) {
      score_matrix[j] = 0;
      direction_matrix[j]   = DIRECTION_NONE;
    }
  }

  for (i=0; i < (seq2_length+1)/nprocs; i++ ) {
    score_matrix[i * ncols ] = 0;
    direction_matrix[i * ncols ]   = DIRECTION_NONE;
  }

  int number_of_chucks= ncols / chunk_size;
  int chunk;
  int *prev_row_buffer;
  prev_row_buffer = (int *) malloc(ncols * sizeof(int) )  ;
  int global_max_rownum=0, max_colnum=0, max_score=0;

/*  for (chunk = 0; chunk < number_of_chucks; chunk++ ) {
    if (rank != ROOT ) {
      Ring_Recv((prev_row_buffer + chunk*chunk_size), chunk_size );
    }
    calculate_chunk(seq1_arr, seq2_arr, score_matrix, direction_matrix, prev_row_buffer,  nrows, ncols, chunk*chunk_size, chunk_size, &max_score, &global_max_rownum, &max_colnum);
    if (rank != (nprocs-1))  {
      Ring_Send((score_matrix + CHUNK_OFFSET(nrows, ncols, chunk, chunk_size)), chunk_size) ;
    }
   }
 */
  MPI_Request send_request ;
//  MPI_Request recv_request ;
  for (chunk = 0; chunk < number_of_chucks; chunk++ ) {
    if (rank != ROOT ) {
      Ring_Recv((prev_row_buffer + chunk*chunk_size), chunk_size);
    }
    calculate_chunk(seq1_arr, seq2_arr, score_matrix, direction_matrix, prev_row_buffer,  nrows, ncols, chunk*chunk_size, chunk_size, &max_score, &global_max_rownum, &max_colnum);
    if (rank != (nprocs-1))  {
      if (chunk != 0) {
	Ring_Wait(&send_request);
      }
      Ring_Isend((score_matrix + CHUNK_OFFSET(nrows, ncols, chunk, chunk_size)), chunk_size, &send_request ) ;
    }
  }




  int *my_results_arr;
  int *global_results_arr;
  global_results_arr = (int *) malloc(3 * sizeof(int) )  ;
  my_results_arr = (int *) malloc(3 * sizeof(int) )  ;

  my_results_arr[0] = max_score;
  my_results_arr[1] = global_max_rownum ;
  my_results_arr[2] = max_colnum ;
  if (DEBUG) {
    printf("%s, RANK%d, My Max_score: %d, i=%d, j=%d \n",program_name, rank,  *my_results_arr, *(my_results_arr+1),*(my_results_arr+2) );
  }

  if (rank == ROOT ) {
    global_results_arr = my_results_arr ;
    Ring_Send(my_results_arr, 3);
  } else if (rank == (nprocs-1))  {
    Ring_Recv(global_results_arr, 3);
    if (DEBUG)
      printf("%s, RANK%d, Compare %d > %d \n",program_name, rank, my_results_arr[0], global_results_arr[0] );
    if (*my_results_arr >= *global_results_arr  ) {
      if (DEBUG)
	printf("%s, RANK%d, My Results are higher \n",program_name, rank);
      global_results_arr = my_results_arr;
    }

  } else {
    Ring_Recv(global_results_arr, 3);
    if (*my_results_arr >= *global_results_arr ) {
      if (DEBUG)
	printf("%s, RANK%d, My Results are higher \n",program_name, rank);
      global_results_arr = my_results_arr;
    }
    Ring_Send(global_results_arr, 3);
  }

  MPI_Bcast(global_results_arr, 3, MPI_INT, (nprocs-1),  MPI_COMM_WORLD);

  max_score = global_results_arr[0];
  global_max_rownum  = global_results_arr[1];
  max_colnum  = global_results_arr[2];


  int lower = (rank * nrows) ;
  int upper = ((rank+1) * nrows) ;
  int align1_length=0;
  int align2_length=0;
//  int *diagonal_ptr ;
  int completion_flag=0 ;

  int *output_meta_data;
  output_meta_data = (int *) malloc(5 * sizeof(int) )  ;

  if (global_max_rownum >= lower && global_max_rownum < upper ) {
    if (DEBUG )
      printf("%s, RANK%d, I have the max score  \n",program_name, rank);
    i = LOCAL_ROW_NUMBER(global_max_rownum,rank, nrows);
    j = max_colnum ;
    backtrace_direciton_matrix(seq1_arr,seq2_arr,direction_matrix,&i,&j, ncols, nrows, align1_arr, align2_arr,&align1_length, &align2_length, &completion_flag );
    if (DEBUG ) {
      printf("%s, RANK%d, After Backtrace, align1_length:%d , align2_length: %d , completion_flag  %d, i = %d, j = %d, GLOBAL_ROW_NUMBER =%d  \n",program_name, rank,  align1_length,align2_length, completion_flag, i, j,  (GLOBAL_ROW_NUMBER(0,rank, nrows) ) );
      for (i=0; i < align1_length; i++ ) {
	printf("%s, RANK%d, letter %c  \n",program_name, rank,  alphabet[align1_arr[i]]  );
      }
    }

    if (DEBUG )
      printf("%s, RANK%d, SENDING  \n",program_name, rank);
    Ring_Reverse_Send(align1_arr, seq1_length );
    Ring_Reverse_Send(align2_arr, seq2_length );

    output_meta_data[0]= completion_flag ;
    if (!completion_flag) {
      output_meta_data[1]= align1_length ;
      output_meta_data[2]= align2_length ;
      output_meta_data[3]= GLOBAL_ROW_NUMBER(0,rank, nrows) - 1    ;
      output_meta_data[4]= j ;
    }
    Ring_Reverse_Send(output_meta_data, 5 );
  } else if ( global_max_rownum <  lower ) {
    if (DEBUG )
      printf("%s, RANK%d, I Above the threshold  \n",program_name, rank);
  } else {
    if (DEBUG )
      printf("%s, RANK%d, RECVING  \n",program_name, rank);
    Ring_Reverse_Recv(align1_arr,  seq1_length );
    Ring_Reverse_Recv(align2_arr, seq2_length );
    Ring_Reverse_Recv(output_meta_data, 5 );

    completion_flag = output_meta_data[0] ;
    align1_length =    output_meta_data[1] ;
    align2_length = output_meta_data[2] ;
    if (DEBUG )
      printf("%s, RANK%d, REC'D, completion_flag = %d \n",program_name, rank, completion_flag );
    if (!completion_flag) {
      if (DEBUG )
	printf("%s, RANK%d, doing b a c k t r a c e  \n",program_name, rank  );
//     i = LOCAL_ROW_NUMBER(output_meta_data[3] ,rank, nrows);
      i = LOCAL_ROW_NUMBER(output_meta_data[3], rank, nrows)  ;
      j = output_meta_data[4] ;
      backtrace_direciton_matrix(seq1_arr,seq2_arr,direction_matrix,&i,&j, ncols,nrows, align1_arr, align2_arr,&align1_length, &align2_length, &completion_flag );
      if (DEBUG )
	printf("%s, RANK%d, After backtrace_direciton_matrix, align1_length:%d , align2_length: %d , completion_flag  %d, i =%d, j=%d \n",program_name, rank,  align1_length,align2_length, completion_flag, i, j);
    }
    if (DEBUG )  {
      for (i=0; i < align1_length; i++ ) {
	printf("%s, RANK%d, letter %c  \n",program_name, rank,  alphabet[align1_arr[i]]  );
      }
    }

    if (rank != ROOT ) {
      if (DEBUG )
	printf("%s, RANK%d, SENDING  \n",program_name, rank);
      Ring_Reverse_Send(align1_arr, seq1_length );
      Ring_Reverse_Send(align2_arr, seq2_length );

      output_meta_data[0]= completion_flag ;
      if (!completion_flag) {
	output_meta_data[1]= align1_length ;
	output_meta_data[2]= align2_length ;
	output_meta_data[3]= GLOBAL_ROW_NUMBER(0,rank, nrows) - 1    ;
	output_meta_data[4]= j ;
      }
      Ring_Reverse_Send(output_meta_data, 5 );
    }

  }

  if (rank == ROOT) {

    char *align1, *align2  ;
    align1 = (char *) calloc(seq1_length,sizeof(char) )  ;
    align2 = (char *) calloc(seq2_length,sizeof(char) )  ;
//     printf("align1_length = %d , align2_length = %d\n ", align1_length, align2_length ) ;
    for(i=0; i<align1_length; i++){

      align1[align1_length-i-1] =  alphabet[align1_arr[i]] ;
    }
    for(i=0; i<align2_length; i++){
      align2[align2_length-i-1] =  alphabet[align2_arr[i]] ;
    }


  }

  gettimeofday(&total_finish,NULL);
  printf("%s, Time, Columns, Chunk, nprocs\n", program_name) ;
  printf("%s, %f, %d, %d, %d, result \n", program_name, get_time_diff(&total_start, &total_finish), ncols, chunk_size,nprocs ) ;

  MPI_Finalize();

  return 1;
}
