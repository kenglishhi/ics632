#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>


static float get_time_diff(struct timeval *s, struct timeval *e);
int get_block_begin(int block_size, int offset) ;
int get_block_end(int block_size, int offset) ;
int multiply_matrices(double *result, double *a, double *b, int matrix_size) ;

static float get_time_diff(struct timeval *s, struct timeval *e) {
    struct timeval diff_tv;

    diff_tv.tv_usec = e->tv_usec - s->tv_usec;
    diff_tv.tv_sec = e->tv_sec - s->tv_sec;

    if (s->tv_usec > e->tv_usec) {
	diff_tv.tv_usec += 1000000;
	diff_tv.tv_sec--;
    }

    return (float) diff_tv.tv_sec + ((float) diff_tv.tv_usec / 1000000.0);
}

void print_matrices(double *result, double *a, double *b, int matrix_size) {
    double *current_result;
    double *current_a;
    double *current_b;
    int i,j;

    printf("matrix_size = %d \n\n", matrix_size  );
    for (i = 0; i < matrix_size; i++) {
	printf(" [" );
	for (j = 0; j < matrix_size; j++){
	    current_a = a + i*matrix_size + j;
	    printf(" %4.1f", *current_a  );
	}
	printf("] " );
	printf("[" );
	for (j = 0; j < matrix_size; j++){
	    current_b = b + i*matrix_size + j;
	    printf(" %4.1f ", *current_b  );
	}
	printf("] = " );
	printf("[ " );
	for (j = 0; j < matrix_size; j++){
	    current_result = result + i * matrix_size + j;
	    printf(" %4.1f ", *current_result  );
	}
	printf("] " );

	printf("\n" );
    }
}

void test_multiply_matrices() {
    int matrix_size = 3;
    double matrix1[matrix_size][matrix_size];
    double matrix2[matrix_size][matrix_size];
    double result[matrix_size][matrix_size];
    int i, j;
    for (i = 0; i < matrix_size; i++)  {
	for (j = 0; j < matrix_size; j++){
	    matrix1[i][j] = (i+1.0);
	    matrix2[i][j] =  (i +1.0);
	    result[i][j] = 0.0;
	}
    }
    multiply_matrices(&result[0][0], &matrix1[0][0], &matrix2[0][0], matrix_size ) ;
    print_matrices(&result[0][0], &matrix1[0][0], &matrix2[0][0], matrix_size ) ;
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
	    current_a = a + i*matrix_size + k;
	    for (j = 0; j < matrix_size; j++) {
		current_result = result + i*matrix_size + j;
		current_b = b + k*matrix_size + j;
		*current_result += *current_a *  *current_b ;
	    }
	}
    }
    return 1;
}
int multiply_block(double *result, double *matrix1, double *matrix2, int block_size, int number_of_blocks, int i, int j, int k) {
    double *current_result;
    double *current_matrix1;
    double *current_matrix2;
    int a, b, c; 
    int matrix_size = block_size * number_of_blocks; 
    int i_block_offset = i*block_size; 
    int j_block_offset = j*block_size*matrix_size; 
    int k_block_offset_row =  k*block_size ; 
    int k_block_offset_col = k*block_size*matrix_size  ; 
    int b_block_offset; 
    for (a = 0; a < block_size; a++) {
        for (b = 0; b < block_size; b++) {
            current_result = result +  a + i_block_offset + j_block_offset + b * matrix_size  ; 
            b_block_offset = b * matrix_size; 
            for (c = 0; c < block_size; c++) {
                  current_matrix2 = matrix2 + a + i_block_offset + k_block_offset_col + c * matrix_size ;
                  current_matrix1 = matrix1 + c + k_block_offset_row + j_block_offset + b_block_offset;
                  *current_result += *current_matrix1 * *current_matrix2;
            } 
	}
    }
    return 1;
}

int main(int argc,char *argv[])
{

    if (argc < 3) {
	printf("Must supply option for Matrix Size and block size, eg: matrix_multiply.exe 400 32 \n"); return -1;
    }
    int debug = 0;
    if (argc == 4) {
	debug = 1;
    }

    int matrix_size = atoi(argv[1]);
    int block_size = atoi(argv[2]);
    int number_of_blocks = matrix_size / block_size;
    double matrix1[matrix_size][matrix_size];
    double matrix2[matrix_size][matrix_size];
    double result[matrix_size][matrix_size];

    struct timeval start;
    struct timeval end;
    int i, j, k ;

    int seed = 10000;
    srand(seed);

    // POPULATE the array

    for (i = 0; i < matrix_size; i++)  {
	for (j = 0; j < matrix_size; j++){
	    matrix1[i][j] = 0.0 + (double)(( i * matrix_size) + j)  ;
	    matrix2[i][j] = 0.0 + (double)(( i * matrix_size) + j)  ;
	    if (debug) {
		printf("matrix2[%d][%d] = %6.1f, -- ", i,j,matrix2[i][j] ) ;
		printf("value = %6.1f,\n",  1.0 + (double)(( i * matrix_size) + j) ) ;
		printf("\n");
	    }
	    result[i][j] = 0.0;
	}
    }
    gettimeofday(&start,NULL);
    if (debug) { 
        printf("calling  multiply_block(:number_of_blocks = > %d, :block_size => %d)\n", number_of_blocks, block_size); 
    } 
    for (i = 0; i < number_of_blocks; i++)  {
	for (j = 0; j < number_of_blocks; j++){
 	    for (k = 0; k < number_of_blocks; k++){
                 multiply_block(&result[0][0], &matrix1[0][0], &matrix2[0][0],block_size, number_of_blocks,i,j,k); 
	     }
	}
    }

    if (debug) {
	print_matrices(&result[0][0],&matrix1[0][0], &matrix2[0][0],matrix_size);
    }
    gettimeofday(&end,NULL);
    if (debug) { 
       printf("Time difference for block implementation  %.5f seconds\n",  get_time_diff(&start, &end));
    }  else { 
       printf("%d,%d,%.5f\n", matrix_size, block_size, get_time_diff(&start, &end));
    } 
    return 1;
}
