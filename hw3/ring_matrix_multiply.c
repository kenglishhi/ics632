#include <unistd.h>
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>

#define DEBUG 0
#define GLOBAL_I(I,RANK,R) ( (RANK)*(R)+(I) )

double get_time_diff(struct timeval *start, struct timeval *finish)  ;

void Ring_Send(double *buffer, int length) ;
void Ring_Recv(double *buffer, int length) ;

void initialize_matrix_slice(double *matrix_a, double *matrix_b, double *result_matrix, int matrix_size, int row_size, int rank) {
    int i, j;
    for (i=0; i < row_size; i++){
	for (j=0; j < matrix_size; j++){
	    *(matrix_a + i*matrix_size + j) = GLOBAL_I(i,rank,row_size) * 1.0;
	    *(matrix_b + i*matrix_size + j) = (GLOBAL_I(i,rank,row_size) + j) * 1.0;
	    *(result_matrix + i*matrix_size + j) = 0.0;
	}
    }
}

void initialize_matrix(double *matrix_a, double *matrix_b, double *result_matrix, int matrix_size) {
    int i, j;
    for (i=0; i < matrix_size; i++){
        for (j=0; j < matrix_size; j++){
            *(matrix_a + i*matrix_size + j) = i * 1.0;
            *(matrix_b + i*matrix_size + j) = (i + j) * 1.0;
            *(result_matrix + i*matrix_size + j) = 0.0;
        }
    }
}

void print_matrix_slice(double *matrix_a, double *matrix_b, double *result_matrix, int matrix_size, int row_size, int rank) {
    double *current_result;
    double *current_a;
    double *current_b;
    int i,j;
    printf("matrix_size = %d \n\n", matrix_size  );
    for (i = 0; i < row_size; i++) {
	printf("RANK%d RESULT row[%d] --  [", rank, GLOBAL_I(i,rank,row_size) );
	for (j = 0; j < matrix_size; j++){
	    current_a = matrix_a + i*matrix_size + j;
	    printf(" %.4f ", *current_a  );
	}
	printf("] " );
	printf("[" );
	for (j = 0; j < matrix_size; j++){
	    current_b = matrix_b + i*matrix_size + j;
	    printf(" %.4f ", *current_b  );
	}
	printf("] = " );
	printf("[ " );
	for (j = 0; j < matrix_size; j++){
	    current_result = result_matrix + i * matrix_size + j;
	    printf(" %.4f ", *current_result  );
	}
	printf("] " );

	printf("\n" );
    }
}

void best_matrix_multiply(double *a, double *b, double *result,  int matrix_size) {
    double *current_result;
    double *current_a;
    double *current_b;
    int i,j,k;
    for (i = 0; i < matrix_size; i++) {
        for (k = 0; k < matrix_size; k++) {
            current_a = a + i*matrix_size + k;
            for (j = 0; j < matrix_size; j++) {
                current_result = result + i*matrix_size + j;
                current_b = b + k*matrix_size + j;
                *current_result += *current_a *  *current_b ;
            }
        }
    }
}


void Ring_Send(double *buffer, int length) { 
    int rank, nprocs, dest; 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    if (rank == (nprocs -1)) { 
       dest = 0 ;
    }  else { 
       dest = rank + 1; 
    } 
/*
   int i;
   for (i = 0; i < length; i++) {
       printf("%d\t-> %d, SendBuffer[%d] = %d\n", rank,dest, i, buffer[i] );
    }
*/

    MPI_Send(buffer, length, MPI_DOUBLE,  dest, 0, MPI_COMM_WORLD); 
} 


void Ring_Recv(double *buffer, int length) { 

    int rank, nprocs, src; 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Status status ; 
    if (rank == 0 )  { 
       src = nprocs -1 ;
    }  else { 
       src = rank - 1; 
    } 
    MPI_Recv(buffer, length, MPI_DOUBLE, src, 0, MPI_COMM_WORLD, &status); 

/*

    int i; 
    for (i = 0; i < length; i++) {
       printf("%d\t<- %d, RecvBuffer[%d] = %d\n", rank,src, i, buffer[i]);
    }
*/
} 

double get_time_diff(struct timeval *start, struct timeval *finish) {
   return (((finish->tv_sec*1000000.0+finish->tv_usec) - (start->tv_sec*1000000.0+start->tv_usec)) / 1000000.00);
/*    struct timeval diff_tv;

    diff_tv.tv_usec = e->tv_usec - s->tv_usec;
    diff_tv.tv_sec = e->tv_sec - s->tv_sec;

    if (s->tv_usec > e->tv_usec) {
        diff_tv.tv_usec += 1000000;
        diff_tv.tv_sec--;
    }
    return (float) diff_tv.tv_sec + ((float) diff_tv.tv_usec / 1000000.0);
*/
}



