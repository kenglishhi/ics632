#include <unistd.h>
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <sys/time.h>

// for L : write a program to measure -- L Time to send a measure = L + b * (message size) where message size = 0, send one to the other and then divide by 2
// for b : write a program to bandwidth -- send a big message using b above
// for w : write a program to measure w -- Time to do work
//

double get_time_diff(struct timeval *start, struct timeval *finish) ;
void initialize_matrix_slice(double *matrix_a, double *matrix_b, double *result_matrix, int matrix_size, int row_size, int rank) ;
void slice_matrix_multiply(double *matrix_a,double *matrix_b, double *result_matrix, int matrix_size, int row_size,  int rank, int nprocs, int step) ;

int main(int argc, char **argv) {
    // Initialize the MPI, get the size and the rank.

    MPI_Init(&argc, &argv);
    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    if (argc < 2) {
        printf("Must supply option for Matrix Size, eg: naive.exe 1000 \n");
        return -1;
    }

    int matrix_size; 
    if (sscanf(argv[1], "%d", &matrix_size) != 1) {
        printf("Invalid command line argument: '%s'\n", argv[1] );
        return 1;
    }
    printf("Matrix size = %d\n", matrix_size ); 

    struct timeval total_start;
    struct timeval total_finish;

    if (rank == 0) {

	int row_size = matrix_size ;

	double *matrix_a, *matrix_b, *result_matrix, *tempR, *tempS ;
	matrix_a = (double *) malloc(matrix_size * (row_size) * sizeof(double)) ;
	matrix_b = (double *) malloc(matrix_size * (row_size) * sizeof(double)) ;
	result_matrix = (double *) malloc(matrix_size * (row_size) * sizeof(double)) ;
	tempR = (double *) malloc(matrix_size * (row_size) * sizeof(double)) ;

	initialize_matrix_slice(matrix_a, matrix_b, result_matrix, matrix_size, row_size, rank);

	tempS = matrix_b;
	gettimeofday(&total_start,NULL);
	slice_matrix_multiply(matrix_a,tempS, result_matrix, matrix_size, row_size,  rank, nprocs, 0)  ;
	gettimeofday(&total_finish,NULL);

	printf("Matrix,Row Size,Time,n,r-squared\n" ) ;
	printf("w = t / r-squared\n" ) ;
	printf("%d,%d,%.4f,%d,%d \n", matrix_size,row_size, get_time_diff(&total_start, &total_finish),1,(row_size*row_size) ) ;
	printf("w = t / r-squared\n" ) ;
	printf("%.8f nanosecs per unit work\n", (get_time_diff(&total_start, &total_finish) *1000000.0) /(row_size*row_size) ) ;
      
    }
    MPI_Finalize();
    return 1;

}
