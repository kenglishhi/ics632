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
#define LOCAL_ROW_NUMBER(GLOBAL_ROW_NUM,RANK,ROW_SIZE) ( GLOBAL_ROW_NUM - (RANK)*(ROW_SIZE) )
#define ARRAY_OFFSET(I,J,NROWS)  ((I)*(NROWS) + (J))





void Ring_Send(int *, int ) ;
void Ring_Recv(int *, int ) ;

double get_time_diff(struct timeval *, struct timeval *);

void calculate_chunk(int *, int *, int *, int *, int *, int ,int , int , int , int *, int *, int *) ; 
void generate_random_array(int *, int, int); 
void print_score_matrix(int *, int , int ) ; 

int  main(int argc,char *argv[]) { 
  /* Initialize MPI */ 
  MPI_Init(&argc, &argv);  
  int rank, nprocs ;  

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);


  struct timeval total_start;
  struct timeval total_finish;

  gettimeofday(&total_start,NULL);


  char  alphabet[21] = "acdefghiklmnpqrstvwy"; 
  char *program_name; 
  program_name = "sw_ring_random_sync";
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
  
  printf("%s, %d, nrows = %d, ncols = %d, chunk_size = %d, seq1_length = %d \n", program_name, rank, nrows, ncols, chunk_size, seq1_length); 

  if (DEBUG)  
    printf("%s, %d, nrows = %d, ncols = %d\n", program_name, rank, nrows, ncols); 
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

//  int chunk_size = 10; // (ncols)/2;
  if (((ncols) % chunk_size) != 0 ) {
    printf("Current program limitation, ncols (%d) must be 2 (%d)  \n", (ncols), nprocs );
    return 1;
  }


  /*  Set the Chunk Length */ 


  srand(time(0)) ;
  int i,j; 
  int *seq1_arr, *seq2_arr, *output1_arr, *output2_arr;
  // all processes will have a copy of Sequence 1
  // They will have a slice of sequence 2
  seq1_arr = (int *) malloc(seq1_length * sizeof(int) )  ;
  seq2_arr = (int *) malloc(seq2_length * sizeof(int) )  ;
  output1_arr = (int *) malloc(seq1_length * sizeof(int) )  ;
  output2_arr = (int *) malloc(seq2_length * sizeof(int) )  ;
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
  int global_max_rownum=0, max_colnum=0, max_score=0;

  for (chunk = 0; chunk < number_of_chucks; chunk++ ) {
    if (rank != ROOT ) { 
//      if (DEBUG)
//        printf("{%d} Receiving columns %d \n", rank, chunk*chunk_size) ;
      Ring_Recv((prev_row_buffer + chunk*chunk_size), chunk_size ); 
    } 
    calculate_chunk(seq1_arr, seq2_arr, score_matrix, direction_matrix, prev_row_buffer,  nrows, ncols, chunk*chunk_size, chunk_size, &max_score, &global_max_rownum, &max_colnum);
    if (rank != (nprocs-1))  { 
//      if (DEBUG)
//        printf("{%d} Sending columns %d \n", rank, chunk*chunk_size) ;
      Ring_Send((score_matrix + CHUNK_OFFSET(nrows, ncols, chunk, chunk_size)), chunk_size) ; 
    } 
//       MPI_Send((score_matrix + CHUNK_OFFSET(nrows, ncols, chunk, chunk_size)), chunk_size, MPI_INT,  dest, 0, MPI_COMM_WORLD);
  }
  if (DEBUG) { 
    print_score_matrix(score_matrix,  nrows, ncols  );
    printf("%s, %d, Max_score: %d, score: %d,  max_colnum: %d \n",program_name, rank,  max_score,  global_max_rownum, max_colnum);
  } 
  printf("%s, %d, Max_score: %d, score: %d,  max_colnum: %d \n",program_name, rank,  max_score,  global_max_rownum, max_colnum);
  int global_max_score;  
  int *my_results_arr;  
  int *global_results_arr;  
  global_results_arr = (int *) malloc(3 * sizeof(int) )  ;
  my_results_arr = (int *) malloc(3 * sizeof(int) )  ;

  my_results_arr[0] = max_score; 
  my_results_arr[1] = global_max_rownum ; 
  my_results_arr[2] = max_colnum ; 
  printf("%s, %d, My Max_score: %d, i=%d, j=%d \n",program_name, rank,  max_score , global_max_rownum ,max_colnum  );
  printf("%s, %d, My Max_score: %d, i=%d, j=%d \n",program_name, rank,  *my_results_arr, *(my_results_arr+1),*(my_results_arr+2) );

  if (rank == ROOT ) { 
    global_results_arr = my_results_arr ; 
    Ring_Send(my_results_arr, 3); 
  } else if (rank == (nprocs-1))  { 
    Ring_Recv(global_results_arr, 3); 
    printf("%s, %d, Compare %d > %d \n",program_name, rank, my_results_arr[0], global_results_arr[0] );
    if (*my_results_arr >= *global_results_arr  ) { 
       printf("%s, %d, My REsults are higher \n",program_name, rank);
       global_results_arr[0] = my_results_arr[0]; 
       global_results_arr[1] = my_results_arr[1]; 
       global_results_arr[2] = my_results_arr[2]; 
    } 

  } else { 
    Ring_Recv(global_results_arr, 3); 
    if (*my_results_arr >= *global_results_arr ) { 
       printf("%s, %d, My REsults are higher \n",program_name, rank);
       global_results_arr[0] = my_results_arr[0]; 
       global_results_arr[1] = my_results_arr[1]; 
       global_results_arr[2] = my_results_arr[2]; 
    } 
    Ring_Send(global_results_arr, 3); 
  } 
     
  MPI_Bcast(global_results_arr, 3, MPI_INT, (nprocs-1),  MPI_COMM_WORLD); 

  max_score = global_results_arr[0]; 
  global_max_rownum  = global_results_arr[1]; 
  max_colnum  = global_results_arr[2]; 

  printf("%s, %d, Global Max_score: %d,max_rownum:%d, max_colnum: \n",program_name, rank,  max_score, global_max_rownum, max_colnum  );

  int lower = (rank * nrows) ; 
  int upper = ((rank+1) * nrows) ; 
  int align1_index=0;
  int align2_index=0;
  int *diagonal_ptr ; 

  printf("%s, %d, max_rownum: %d Lower: %d, Upper: %d \n",program_name, rank, global_max_rownum, lower, upper  );

  if (global_max_rownum >= lower && global_max_rownum < upper ) { 
     printf("%s, %d, Global MaX ROWS IS MINE!: %d, Local Row Number %d \n",program_name, rank,  global_max_rownum, LOCAL_ROW_NUMBER(global_max_rownum,rank, nrows)  );
     i = LOCAL_ROW_NUMBER(global_max_rownum,rank, nrows); 
     j = max_colnum ; 
     printf("%s, %d, ARRAY_OFFSET(%d, %d, %d) == %d \n",program_name, rank,  i,j, ncols , ARRAY_OFFSET(i, j, ncols)  );
      
     while (1) { 
        diagonal_ptr = (direction_matrix + ARRAY_OFFSET(i, j, ncols)) ; 
        if (*diagonal_ptr == DIRECTION_NONE  ) { 
            printf("%s, %d, No Direction!!!!!\n", program_name, rank  ) ;  
            break ;
        } 
        if (i == 0 ) {
            printf("%s, %d, i == 0 !!!!!\n", program_name, rank  ) ;  
            break ;
        } 

        if (*diagonal_ptr ==  DIRECTION_DIAGONAL ) {
            printf("%s, %d, [%d,%d] DIRECTION_DIAGONAL !!!!!\n", program_name, rank, i, j   ) ;  
            output1_arr[align1_index] = seq1_arr[j-1]  ;
            output2_arr[align2_index] = seq2_arr[i-1]  ;
            i--; j--;
        }
        else if (*diagonal_ptr ==  DIRECTION_LEFT  ) {
            printf("%s, %d, DIRECTION_LEFT !!!!!\n", program_name, rank  ) ;  
            output1_arr[align1_index] = seq1_arr[j-1]  ;
            output2_arr[align2_index] = -1 ;
            j--;
        }
        else if (*diagonal_ptr  ==  DIRECTION_UP ) {
            printf("%s, %d, DIRECTION_UP !!!!!\n", program_name, rank  ) ;  
            output1_arr[align1_index] = -1;
            output2_arr[align2_index] = seq2_arr[i-1]  ;
            i--;
        }
        align1_index++; align2_index++;
     } 
     
  } 
  



  gettimeofday(&total_finish,NULL);
  printf("%s, %d, %f seconds to complete work\n", program_name, rank, get_time_diff(&total_start, &total_finish) ) ;
  printf("-----------------------------------------\n" ) ;

  
  MPI_Finalize(); 
    
  return 1; 
}
