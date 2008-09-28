#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#define INDEX(X,Y,SIZE) ((X)*(SIZE))+(Y)


float get_time_diff(struct timeval *s, struct timeval *e) ;

/*****************************
** jacobi iteration formula:
**    n[i,j] = 0.25 * (m[i+1, j] +  m[i-1,j] + m[i,j+1] + m[i,j-1]
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
	    printf(" %4.3f ", *current_result  );
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

    struct timeval section_start;
    struct timeval section_end;

    double *output;
    double *init;
    double *temp;
    int output_size = size + 2;
    init = (double *) malloc(output_size * (output_size) * sizeof(double));
    output = (double *) malloc(output_size * (output_size) * sizeof(double));
    int i, j;

    for (i=0; i < output_size ; i++){
	output[INDEX(i, 0, output_size) ] = 1.0;
	init[ INDEX(i, 0, output_size) ] = 1.0;
	for (j=1; j < output_size ; j++){
	    init[  INDEX(i, j, output_size)   ] = 0.0;
	    output[INDEX(i, j, output_size) ] = 0.0;
	}
    }
    if (debug) {
	print_matrix(init, output_size);
	print_matrix(output, output_size);
    }

    gettimeofday(&section_start,NULL);

    int iteration =0 ;
    int i_prev_offset, i_next_offset;
    do {
	for (i=1; i <=size; i++) {
	    i_prev_offset = i-1; i_next_offset = i+1;
	    for (j=1; j <= size; j++) {
		output[INDEX(i, j, output_size) ] = 0.25 *  (init[INDEX(i_prev_offset, j, output_size)]  +  init[INDEX(i_next_offset, j, output_size) ] + init[INDEX(i,j+1, output_size ) ] + init[INDEX(i,j-1,output_size )]  ) ;
	    }
	}
	iteration++;
        // do a pointer swap
        if (iteration < number_of_iterations ) {  
          temp = init; 
          init = output; 
          output = temp; 
        } 
        
    }  while (iteration < number_of_iterations ) ; 
    gettimeofday(&section_end,NULL);

    if (debug) {
	printf("output_size = %d\n", output_size);
	print_matrix(output, output_size );
    } else {
	printf("Matrix Size, Iterations, Time Spent\n");
	printf("%d,%d,%f\n", size, number_of_iterations, get_time_diff(&section_start, &section_end) );
    }
    return 1;
}
