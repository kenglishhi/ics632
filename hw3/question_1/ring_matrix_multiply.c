#include <unistd.h>
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>


#define GLOBAL_I(I,RANK,R) ( (RANK)*(R)+(I) )

void Ring_Send(int *buffer, int length) ;
void Ring_Recv(int *buffer, int length) ;
void initialize_matrices(int *matrix_a, int *matrix_b, int *result_matrix, int matrix_size, int row_size, int rank) {
    int i, j;
    for (i=0; i < row_size; i++){
	for (j=0; j < matrix_size; j++){
	    *(matrix_a + i*matrix_size + j) = GLOBAL_I(i,rank,row_size) * 1.0;
	    *(matrix_b + i*matrix_size + j) = (GLOBAL_I(i,rank,row_size) + j) * 1.0;
	    *(result_matrix + i*matrix_size + j) = 0.0;
	}
    }
}

void print_matrices(int *matrix_a, int *matrix_b, int *result_matrix, int matrix_size, int row_size, int rank) {
    int *current_result;
    int *current_a;
    int *current_b;
    int i,j;
    printf("matrix_size = %d \n\n", matrix_size  );
    for (i = 0; i < row_size; i++) {
	printf("RANK%d RESULT row[%d] --  [", rank, GLOBAL_I(i,rank,row_size) );
	for (j = 0; j < matrix_size; j++){
	    current_a = matrix_a + i*matrix_size + j;
	    printf(" %d ", *current_a  );
	}
	printf("] " );
	printf("[" );
	for (j = 0; j < matrix_size; j++){
	    current_b = matrix_b + i*matrix_size + j;
	    printf(" %d ", *current_b  );
	}
	printf("] = " );
	printf("[ " );
	for (j = 0; j < matrix_size; j++){
	    current_result = result_matrix + i * matrix_size + j;
	    printf(" %d ", *current_result  );
	}
	printf("] " );

	printf("\n" );
    }
}



int main(int argc, char **argv) {
    int rank, nprocs;
    // Initialize the MPI, get the size and the rank.
    if (argc < 2) {
	printf("Must supply option for Matrix Size, eg: naive.exe 1000 \n");
	return -1;
    }

    int matrix_size = atoi(argv[1]);

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    int row_size = matrix_size / nprocs;

    int *matrix_a; int *matrix_b; int *result_matrix ; int *tempR; int *tempS; int *tempX;
    matrix_a = (int *) malloc(matrix_size * (row_size) * sizeof(int)) ;
    matrix_b = (int *) malloc(matrix_size * (row_size) * sizeof(int)) ;
    result_matrix = (int *) malloc(matrix_size * (row_size) * sizeof(int)) ;
    tempR = (int *) malloc(matrix_size * (row_size) * sizeof(int)) ;

    initialize_matrices(matrix_a, matrix_b, result_matrix, matrix_size,row_size, rank);

    // step 1
    int i, j, k, l ;
    int step ;
    printf("%d\tr = %d\n", rank, row_size);

    int p = nprocs;
    int q = rank;
    int *current_a, *current_b, *current_result;
    int global_i;
    int offset;
//    for (step = 0 ; step < nprocs; step++)
    tempS = matrix_b;
    for (step = 0 ; step < nprocs; step++) {
	Ring_Send(tempS, row_size * matrix_size) ;
	Ring_Recv(tempR, row_size * matrix_size) ;
	for (l=0; l < nprocs ; l++) {
	    printf("%d l =  %d  \n", rank, l);
	    for (i =0; i < row_size; i++) {
		for (k =0; k < row_size; k++) {
		    for (j =0; j < row_size; j++) {
			global_i = GLOBAL_I(i,rank,row_size);
			i = i;
//                 if (rank ==0 ){
			current_result = result_matrix + matrix_size * i + l*row_size + j ;
//                 printf("RANK%d\tc[%d, %d] += ", rank, global_i, l*row_size +j  ) ;
//
			if ((q-step) > 0) {
			    offset = q-step;
			} else {
			    offset = step - q;
			}
			current_a = matrix_a + matrix_size*i + row_size * ((offset)%p) +k ;
			current_b = tempS + matrix_size*k + l*row_size +j;

			printf("RANK%d\tSTEP%d\ta[%d, %d]:(%d) * b[%d,%d]: (%d)   ", rank,step, global_i, (row_size * ((offset)%p) +k ), *current_a, k,(l*row_size +j), *current_b   ) ;
			printf("current (%d)   ", *current_result   ) ;
//                 printf("b[%d, %d] (%d) ", k, l*row_size +j, *current_b   ) ;
			printf("\n" ) ;
			*current_result +=  (*current_a) * (*current_b);
//                }
		    }
		}
	    }
	}
//      print_matrices(matrix_a, tempS, result_matrix, matrix_size, row_size, rank);
	tempX = tempS;
	tempS = tempR;
	tempR = tempX;


    }

//    if (rank == 1 ){
//
//      print_matrices(matrix_a, matrix_b, result_matrix, matrix_size);
//    }

    print_matrices(matrix_a, matrix_b, result_matrix, matrix_size, row_size, rank);
    MPI_Finalize();

    return 1;

}
