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
double get_validated_result(int i, int j, int matrix_size) {
   int k; 
   double result =0.0;
   double a=0.0; 
   double b=0.0; 
   for (k=0; k< matrix_size; k++ ) {

       a = i * 1.0;  
       b = (j + k) * 1.0;  
       result +=  a * b; 
   } 
   return result;

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


void slice_matrix_multiply(double *matrix_a,double *matrix_b, double *result_matrix, int matrix_size, int row_size,  int rank, int nprocs, int step) {
    int l, i, j, k, global_i; 
    double *current_a, *current_b, *current_result;

        for (l=0; l < nprocs ; l++) {
            if (DEBUG) {
                printf("%d l =  %d  \n", rank, l);
            }
            for (i =0; i < row_size; i++) {
                for (k =0; k < row_size; k++) {
                    current_a = matrix_a + (matrix_size * i) + row_size * ((nprocs-step + nprocs) % nprocs) + k ;
                    for (j =0; j < row_size; j++) {

                        current_result = result_matrix + (matrix_size * i) + (l * row_size) + j ;
                        current_b = matrix_b + (matrix_size*k) + (l*row_size) +j;

                        if (DEBUG) {
                            global_i = GLOBAL_I(i,rank,row_size);
                            printf("RANK%d\tSTEP%d\ta[%d, %d]:(%f) * b[%d,%d]: (%f)   ", rank,step, global_i, (row_size * ((nprocs-step + nprocs)%nprocs) +k ), *current_a, k,(l*row_size +j), *current_b   ) ;
                            printf("current (%f)   ", *current_result   ) ;
                            printf("\n" ) ;
                        }
                        *current_result +=  (*current_a) * (*current_b);
                    }
                }
            }
        }
} 

void slice_matrix_multiply_openmp(double *matrix_a,double *matrix_b, double *result_matrix, int matrix_size, int row_size,  int rank, int nprocs, int step) {
    int l, i, j, k, global_i;
    double *current_a, *current_b, *current_result;

        for (l=0; l < nprocs ; l++) {
            if (DEBUG) {
                printf("%d l =  %d  \n", rank, l);
            }
            for (i =0; i < row_size; i++) {
                for (k =0; k < row_size; k++) {
                    current_a = matrix_a + (matrix_size * i) + row_size * ((nprocs-step + nprocs) % nprocs) + k ;
#pragma omp parallel for private(j)                     
                    for (j =0; j < row_size; j++) {

                        current_result = result_matrix + (matrix_size * i) + (l * row_size) + j ;
                        current_b = matrix_b + (matrix_size*k) + (l*row_size) +j;

                        if (DEBUG) {
                            global_i = GLOBAL_I(i,rank,row_size);
                            printf("RANK%d\tSTEP%d\ta[%d, %d]:(%f) * b[%d,%d]: (%f)   ", rank,step, global_i, (row_size * ((nprocs-step + nprocs)%nprocs) +k ), *current_a, k,(l*row_size +j), *current_b   ) ;
                            printf("current (%f)   ", *current_result   ) ;
                            printf("\n" ) ;
                        }
                        *current_result +=  (*current_a) * (*current_b);
                    }
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
    printf("%d\t<- %d, Doing Send\n", rank, dest);
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

    printf("%d\t<- %d, Doing Receive\n", rank,src);
    int i; 
    for (i = 0; i < length; i++) {
       printf("%d\t<- %d, RecvBuffer[%d] = %d\n", rank,src, i, buffer[i]);
    }
*/
}

void Ring_Isend(double *buffer, int length, MPI_Request *request) { 
    int rank, nprocs, dest; 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    if (rank == (nprocs -1)) { 
       dest = 0 ;
    }  else { 
       dest = rank + 1; 
    } 

    MPI_Isend(buffer, length, MPI_DOUBLE,  dest, 0, MPI_COMM_WORLD,request); 
}


void Ring_Irecv(double *buffer, int length, MPI_Request *request ) { 

    int rank, nprocs, src; 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    if (rank == 0 )  { 
       src = nprocs -1 ;
    }  else { 
       src = rank - 1; 
    } 
    MPI_Irecv(buffer, length, MPI_DOUBLE, src, 0, MPI_COMM_WORLD, request); 
} 
void Ring_Wait(MPI_Request *request) { 
    MPI_Status status  ;
    MPI_Wait(request, &status );
} 


double get_time_diff(struct timeval *start, struct timeval *finish) {
    // Copied from Casanova
   return (((finish->tv_sec*1000000.0+finish->tv_usec) - (start->tv_sec*1000000.0+start->tv_usec)) / 1000000.00);
}




