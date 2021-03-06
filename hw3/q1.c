#include <unistd.h>
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <sys/time.h> 

#define DEBUG 0
#define GLOBAL_I(I,RANK,R) ( (RANK)*(R)+(I) )


void Ring_Send(double *buffer, int length) ;
void Ring_Recv(double *buffer, int length) ; 
void initialize_matrix_slice(double *matrix_a, double *matrix_b, double *result_matrix, int matrix_size, int row_size, int rank) ; 
void initialize_matrix(double *matrix_a, double *matrix_b, double *result_matrix, int matrix_size) ; 
void print_matrix_slice(double *matrix_a, double *matrix_b, double *result_matrix, int matrix_size, int row_size, int rank) ; 
void best_matrix_multiply(double *a, double *b, double *result,  int matrix_size)  ; 
double get_time_diff(struct timeval *start, struct timeval *finish) ; 
void slice_matrix_multiply(double *matrix_a,double *matrix_b, double *result_matrix, int matrix_size, int row_size,  int rank, int nprocs, int step) ; 
double get_validated_result(int i, int j, int matrix_size) ;


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
    int i, j ;
    int step ;
    if (DEBUG) {
	printf("%d\tr = %d\n", rank, row_size);
    }

    double validate_result ; 
    double diff ; 
    double  *current_result;

    tempS = matrix_b;

    for (step = 0 ; step < nprocs; step++) {
        if (rank%2 == 0 ) { 
  	  Ring_Send(tempS, row_size * matrix_size) ;
          Ring_Recv(tempR, row_size * matrix_size) ;
        } else {
	  Ring_Recv(tempR, row_size * matrix_size) ;
	  Ring_Send(tempS, row_size * matrix_size) ;
        } 
        slice_matrix_multiply(matrix_a,tempS, result_matrix, matrix_size, row_size,  rank, nprocs, step)  ; 

	tempX = tempS;
	tempS = tempR;
	tempR = tempX;
    }


    if (DEBUG) {
	print_matrix_slice(matrix_a, matrix_b, result_matrix, matrix_size, row_size, rank);
    }

    gettimeofday(&total_finish,NULL); 

    if (rank ==(nprocs-1) ) { 
        printf("RANK,CompleteTime,row_size, matrix_size\n" ) ;
    } 
    printf("%d,%.8f,%d, %d\n", rank, get_time_diff(&total_start, &total_finish),row_size, matrix_size ) ;
    return 1;
    double *verify_matrix_a, *verify_matrix_b, *verify_result_matrix, *global_current_result ;

    for (i=0; i < row_size; i++){
	for (j=0; j < matrix_size; j++){
	    current_result = result_matrix + i * matrix_size + j;
            validate_result = get_validated_result( GLOBAL_I(i,rank,row_size)  , j , matrix_size); 
            diff  =  *current_result - validate_result ; // *global_current_result ; 
            if (diff != 0.0) { 
	        printf(" (%.4f == %.4f) => diff = %.4f ", *current_result, validate_result, diff );
            } 
        } 
    } 
 

    gettimeofday(&total_start,NULL); 

    verify_matrix_a = (double *) malloc(matrix_size * (matrix_size) * sizeof(double)) ;
    verify_matrix_b = (double *) malloc(matrix_size * (matrix_size) * sizeof(double)) ;
    verify_result_matrix = (double *) malloc(matrix_size * (matrix_size) * sizeof(double)) ;

    initialize_matrix(verify_matrix_a, verify_matrix_b, verify_result_matrix, matrix_size); 
    best_matrix_multiply(verify_matrix_a, verify_matrix_b, verify_result_matrix, matrix_size); 
    if (DEBUG && (rank ==0) ) { 
       print_matrix_slice(verify_matrix_a, verify_matrix_b, verify_result_matrix, matrix_size, matrix_size,rank);
    } 

    for (i=0; i < row_size; i++){
	for (j=0; j < matrix_size; j++){
	    current_result = result_matrix + i * matrix_size + j;
	    global_current_result = verify_result_matrix  + GLOBAL_I(i,rank,row_size) * matrix_size + j;
            diff  =  *current_result - *global_current_result ; 
//            if (diff != 0.0) { 
//	        printf(" (%.4f == %.4f) => diff = %.4f ", *current_result, *global_current_result, diff );
//            } 
        } 
    } 
    gettimeofday(&total_finish,NULL); 

    printf("RANK%d,Naive,%.8f\n", rank, get_time_diff(&total_start, &total_finish) ) ;

    MPI_Finalize();

    return 1;

}
