/* sw_paralle.c */

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



void Ring_Send(int *, int ) ;
void Ring_Recv(int *, int ) ;


void calculate_chunk(int *, int *, int *, int *, int *, int ,int , int , int , int *, int *, int *) ; 
void generate_random_array(int *, int, int); 
void print_score_matrix(int *, int , int ) ; 

int  main(int argc,char *argv[]) { 
  /* Initialize MPI */ 
  MPI_Init(&argc, &argv);  
  int rank, nprocs;  

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  char  alphabet[21] = "acdefghiklmnpqrstvwy"; 

  /* Parse Command Line Args */ 
  int seq1_length, seq2_length;
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

  int nrows =  (seq1_length+1)/nprocs;
  int ncols =  (seq1_length+1);
  
  if (DEBUG)  
    printf("[%d] nrows = %d, ncols = %d\n", rank, nrows, ncols); 
  if (seq1_length != seq2_length) {
    printf("Current program limitation, seq1_length != seq2_length \n");
    return 1;
  }

  if (((ncols) % nprocs) != 0 ) {
    printf("Current program limitation, ncols (%d) must be divisible by nprocs (%d)  \n", (ncols), nprocs );
    return 1;
  }

  if (((ncols) % 2) != 0 ) {
    printf("Current program limitation, ncols (%d) must be 2 (%d)  \n", (ncols), nprocs );
    return 1;
  }

  int chunk_size = 2; // (ncols)/2;
  if (((ncols) % chunk_size) != 0 ) {
    printf("Current program limitation, ncols (%d) must be 2 (%d)  \n", (ncols), nprocs );
    return 1;
  }


  /*  Set the Chunk Length */ 


  srand(time(0)) ;
  int i,j; 
  int *seq1_arr, *seq2_arr;
  // all processes will have a copy of Sequence 1
  // They will have a slice of sequence 2
  seq1_arr = (int *) malloc(seq1_length * sizeof(int) )  ;
  seq2_arr = (int *) malloc(seq2_length * sizeof(int) )  ;
  char *seq1, *seq2; 

  if (rank == 0 ) { 
     generate_random_array(seq1_arr, seq1_length,  20);   
     seq2_arr = seq1_arr; 
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
    printf("Rank is not %d, seq2_length: %d, ncols: %d, nrows: %d \n", rank,  seq2_length, ncols, nrows   );
  if (MPI_Bcast(seq1_arr, seq1_length, MPI_INT, ROOT, MPI_COMM_WORLD) ) {
    printf("Error while calling MPI_Bcast()\n"); 
    exit(0); 
  } 

  if (MPI_Bcast(seq2_arr, seq2_length, MPI_INT, ROOT, MPI_COMM_WORLD) ) {
    printf("Error while calling MPI_Bcast()\n"); 
    exit(0); 
  } 

  int *score_matrix , *direction_matrix ; 
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
  int global_max_rownum, max_j, max_score;

  for (chunk = 0; chunk < number_of_chucks; chunk++ ) {
    if (rank != ROOT ) { 
//      if (DEBUG)
//        printf("{%d} Receiving columns %d \n", rank, chunk*chunk_size) ;
      Ring_Recv((prev_row_buffer + chunk*chunk_size), chunk_size ); 
    } 
    calculate_chunk(seq1_arr, seq2_arr, score_matrix, direction_matrix, prev_row_buffer,  nrows, ncols, chunk*chunk_size, chunk_size, &max_score, &global_max_rownum, &max_j);
    if (rank != (nprocs-1))  { 
//      if (DEBUG)
//        printf("{%d} Sending columns %d \n", rank, chunk*chunk_size) ;
      Ring_Send((score_matrix + CHUNK_OFFSET(nrows, ncols, chunk, chunk_size)), chunk_size) ; 
    } 
//       MPI_Send((score_matrix + CHUNK_OFFSET(nrows, ncols, chunk, chunk_size)), chunk_size, MPI_INT,  dest, 0, MPI_COMM_WORLD);
  }
  if (DEBUG) { 
    print_score_matrix(score_matrix,  nrows, ncols  );
    printf("[%d] Max_score: %d, score: %d,  max_j: %d \n",rank,  max_score,  global_max_rownum, max_j);
  } 
  int global_max_score;  
  MPI_Allreduce(&max_score, &global_max_score, 1, MPI_INT, MPI_MAX,  MPI_COMM_WORLD); 

  if (global_max_score==max_score) { 
    printf("[%d] I HAVE THE MAX : %d,  rownum=%d   \n", rank, global_max_score, global_max_rownum );
  } 
  
  MPI_Finalize(); 
    
  return 1; 
}
