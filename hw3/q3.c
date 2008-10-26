#include <unistd.h>
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <sys/time.h> 

#define DEBUG 0
#define GLOBAL_I(I,RANK,R) ( (RANK)*(R)+(I) )


void Ring_Send(double *buffer, int length) ;
void Ring_Recv(double *buffer, int length) ; 
void Ring_Isend(double *buffer, int length, MPI_Request *request ) ;
void Ring_Irecv(double *buffer, int length, MPI_Request *request ) ; 
void Ring_Wait(MPI_Request *request) ; 
void initialize_matrix_slice(double *matrix_a, double *matrix_b, double *result_matrix, int matrix_size, int row_size, int rank) ; 
void initialize_matrix(double *matrix_a, double *matrix_b, double *result_matrix, int matrix_size) ; 
void print_matrix_slice(double *matrix_a, double *matrix_b, double *result_matrix, int matrix_size, int row_size, int rank) ; 
void best_matrix_multiply(double *a, double *b, double *result,  int matrix_size)  ; 
double get_time_diff(struct timeval *start, struct timeval *finish) ; 
void slice_matrix_multiply(double *matrix_a,double *matrix_b, double *result_matrix, int matrix_size, int row_size,  int rank, int nprocs, int step) ; 

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, nprocs;
    // Initialize the MPI, get the size and the rank.

    if (argc < 2) {
	printf("Must supply option for Matrix Size, eg: naive.exe 1000 \n");
	return -1;
    }

    int matrix_size; 
    if (sscanf(argv[1], "%d", &matrix_size) != 1) { 
        printf("Invalid command line argument: '%s'\n", argv[1] ); 
        return 1; 
    }  
//    int matrix_size = atoi(argv[1]);

    struct timeval total_start; 
    struct timeval total_finish; 

    gettimeofday(&total_start,NULL); 

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    int row_size = matrix_size / nprocs;

    double *matrix_a, *matrix_b, *result_matrix, *tempR, *tempS, *tempX;
    matrix_a = (double *) malloc(matrix_size * (row_size) * sizeof(double)) ;
    matrix_b = (double *) malloc(matrix_size * (row_size) * sizeof(double)) ;
    result_matrix = (double *) malloc(matrix_size * (row_size) * sizeof(double)) ;
    tempR = (double *) malloc(matrix_size * (row_size) * sizeof(double)) ;

    initialize_matrix_slice(matrix_a, matrix_b, result_matrix, matrix_size,row_size, rank);

    // step 1
    int step ;
    if (DEBUG) {
	printf("%d\tr = %d\n", rank, row_size);
    }


    tempS = matrix_b;
    MPI_Request send_request ; 
    MPI_Request recv_request ; 
   
    for (step = 0 ; step < nprocs; step++) {
        Ring_Isend(tempS, row_size * matrix_size, &send_request) ;
	Ring_Irecv(tempR, row_size * matrix_size, &recv_request) ; 
        slice_matrix_multiply(matrix_a,tempS, result_matrix, matrix_size, row_size,  rank, nprocs, step)  ; 
        Ring_Wait(&send_request);  
        Ring_Wait(&recv_request);  

	tempX = tempS;
	tempS = tempR;
	tempR = tempX;
    }


    if (DEBUG) {
	print_matrix_slice(matrix_a, matrix_b, result_matrix, matrix_size, row_size, rank);
    }

    gettimeofday(&total_finish,NULL); 
    if (rank == (nprocs-1) ) { 
        printf("RANK,CompleteTime,row_size, matrix_size\n" ) ;
    } 
    printf("%d,%.8f,%d, %d\n", rank, get_time_diff(&total_start, &total_finish),row_size, matrix_size ) ;

    MPI_Finalize();

    return 1;

}
