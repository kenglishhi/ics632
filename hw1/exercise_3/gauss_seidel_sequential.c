#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

/*****************************
** gauss-siedel iteration formula:
**    output[i,j] = 0.25 * (m[i+1, j] +  output[i-1,j] + m[i,j+1] + output[i,j-1]
**
** I borrowed the algorithm from this web site:
** http://www.netlib.org/utk/papers/mpi-book/node44.html
*******************************/
void print_matrix(double *result, int matrix_size) {
    double *current_result;
    int i,j;

    printf("matrix_size = %d \n\n", matrix_size  );
    for (i = 0; i < matrix_size; i++) {
	printf(" [" );
	for (j = 0; j < matrix_size; j++){
	    current_result = result + i * matrix_size + j;
	    printf(" %4.1f ", *current_result  );
	}
	printf("] " );

	printf("\n" );
    }
}


int main(int argc,char *argv[]) {
    if (argc < 3) {
	printf("Must supply option for Matrix Size and number of iterations size, eg: jacobi_sequential.exe 400 2 \n");
	return -1;
    }
    int debug = 0;
    if (argc == 4) {
	debug = 1;
    }
    int size = atoi(argv[1]);
    int number_of_iterations = atoi(argv[2]);

    double output[size][size];
    double m[size + 2][size + 2];
    int i, j;
//    double term1, term2, term3, term4;

    for (i=0; i<size; i++){
	for (j=0; j<size; j++){
	    m[i+1][j+1] = 0.0;
	    output[i][j] = 0.0;
	}
    }

    int a, b;
    for (i=0; i < size+2; i++) {
	m[i][0] = 1.0;
	for (j=1; j<size+2; j++) {
	    m[i][j] = 0.0;
	}

    }
    print_matrix(&m[0][0], size+2);
    int iteration =0 ;
    double term2, term4; 
    while (iteration < number_of_iterations ) {
	for (i=0; i < size; i++){
	    a = i + 1  ;
	    for (j=0; j < size; j++){
		b = j+1;
                if (i==0 && j==0) { 
                   term2 =  0.25 *m[0][0] ; 
                   term4 =  0.25 *m[0][0] ; 
                }  else {
                    term2 = output[i-1][j] ; 
                    term4 = output[i][j - 1] ; 
                } 
		output[i][j] = 0.25 *  (m[a+1][b] +  term2 + m[a][b+1] + term4) ;
	    }
	}

	for (i=0; i < size; i++){
	    a = i + 1  ;
	    for (j=0; j < size; j++){
		b = j+1;
                m[a][b] = output[i][j] ; 
            } 
        } 
        iteration++; 
    }

    print_matrix(&output[0][0], size);
    return 1;
}
