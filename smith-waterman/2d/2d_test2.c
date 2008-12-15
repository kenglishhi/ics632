/*************************************
 *
 * 2d_test.c
 * Author: Kevin English
 * University of Hawaii
 * ICS 632
 *
 * This program tests some of our 2d functions.
 **/

#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <mpi.h>

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

int get_right_destination() ;
int get_bottom_destination() ;

void Bottom_Send(int *buffer, int length )  ;
void Right_Send(int *buffer, int length ) ;

void Top_Recv(int *buffer, int length ) ;
void Left_Recv(int *buffer, int length ) ;

int isLeftColumnChunk(); 
int isTopRowChunk(); 


int  main(int argc,char *argv[]) { 
  /* Initialize MPI */ 
  MPI_Init(&argc, &argv);  
  int rank, nprocs ;  

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  char *program_name = "2d_test" ; 

  int send_arr[1] ; 
  send_arr[0] = rank; 
  int recv_arr[1] ; 
  recv_arr[0] = rank; 

  printf("%s, RANK%d, isTopRowChunk() = %d ---- isLeftColumnChunk() = %d  \n", program_name, rank, isTopRowChunk(), isLeftColumnChunk() ) ; 

  return 0 ; 

}
