#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>


static float getTimeDiff(struct timeval *s, struct timeval *e);
int get_block_begin(int block_size, int offset) ; 
int get_block_end(int block_size, int offset) ; 

static float getTimeDiff(struct timeval *s, struct timeval *e) {
    struct timeval diff_tv;

    diff_tv.tv_usec = e->tv_usec - s->tv_usec;
    diff_tv.tv_sec = e->tv_sec - s->tv_sec;

    if (s->tv_usec > e->tv_usec) {
	diff_tv.tv_usec += 1000000;
	diff_tv.tv_sec--;
    }

    return (float) diff_tv.tv_sec + ((float) diff_tv.tv_usec / 1000000.0);
}
int get_block_begin(int block_size, int offset) { 
     return (offset * block_size) ;
} 

int get_block_end(int block_size, int offset) { 
    return (offset + 1 ) * block_size - 1;
}
int multiply_matrices(double *result, double *a, double *b, int matrix_size) { 
    double *current_result;
    double *current_a;
    double *current_b;
    int i,j,k;
    for (i = 0; i < matrix_size; i++) {
        for (k = 0; k < matrix_size; k++) {
            for (j = 0; j < matrix_size; j++) {
                current_result = result + i*matrix_size + j;
                current_a = a + i*matrix_size + k;
                current_b = b + k*matrix_size + j;
                *current_result += *current_a *  *current_b ;
            }
        }
    }
}
//int multiple_matrix(int *result[], int *)
int main(int argc,char *argv[])
{

    if (argc < 3) {
	printf("Must supply option for Matrix Size, eg: matrix_multiply.exe 1000 \n");
	return -1;
    }
    int debug = 0;
    if (argc == 4) {
	debug = 1;
    }
    int matrix_size = atoi(argv[1]);
    printf("Matrix_size is  %d\n", matrix_size);

    int block_size = atoi(argv[2]);
    printf("Block Size %d\n", matrix_size);

    double matrix1[matrix_size][matrix_size];
    double matrix2[matrix_size][matrix_size];
    double result[matrix_size][matrix_size];

    struct timeval start;
    struct timeval end;
    double time_elapsed;
    int i,k,j;

    int seed = 10000;
    srand(seed);

    // POPULATE the array
    for (i = 0; i < matrix_size; i++)  {
	for (j = 0; j < matrix_size; j++){
	    matrix1[i][j] = (double) rand();
	    matrix2[i][j] = (double) rand();
	    result[i][j] = 0.0;
	}
    }


    /*
     * #2 : Do the MATRIX Multiplication i-k-j
     */

    gettimeofday(&start,NULL);
    int number_of_blocks = matrix_size/block_size;

    printf("Number of Blocks: %d\n", number_of_blocks);

    printf("\nHere we go \n");
    char *implementation = "i-k-j";
    int inner_i, inner_j, inner_k;
    int a,b,c;
    int block_begin_a, block_end_a;
    int block_begin_b, block_end_b;
    int block_begin_c, block_end_c;
    int total_multiplications = 0; 

    if (0){  

    for (a=0; a < number_of_blocks; a++ ) {
	block_begin_a = get_block_begin(block_size, a); 
	block_end_a = get_block_end(block_size, a) ; 
	if (debug) {
	    printf("Block Number: %d, Block Begins: %d, Block Ends: %d \n",a, block_begin_a, block_end_a);
	}
	for (i=block_begin_a; i <= block_end_a; i++ ) {
          for (b=0; b < number_of_blocks; b++ ) {
	    block_begin_b = get_block_begin(block_size, b); 
            block_end_b = get_block_end(block_size, b) ; 
	    for (k=block_begin_b; k <= block_end_b; k++ ) {
              for (c=0; c < number_of_blocks; c++ ) {
	        block_begin_c = get_block_begin(block_size, c); 
                block_end_c = get_block_end(block_size, c) ; 
		for (j=block_begin_c; j <= block_end_c; j++ ) {
		    result[i][j] += matrix1[i][k] * matrix2[k][j];
                    total_multiplications++;
	            if (debug) 
		        printf("i,k,j = %d,%d,%d\n", i, k, j);
		}
	      }
            }
          }
	}

    }
    } else { 
      for (a=0; a < number_of_blocks; a++ ) {
        for (b=0; b < number_of_blocks; b++ ) {
          for (c=0; c < number_of_blocks; c++ ) {
             multiply_matrices(&result[a][0], &matrix1[b][0], &matrix2[c][0], block_size) ; 
          }
        }
      }
    } // end if (0)
    gettimeofday(&end,NULL);
    printf("Time difference for %s implementation %.5f seconds\n", implementation, getTimeDiff(&start, &end));
    printf("Total Multiplications : %d\n", total_multiplications);

    return 1;
}