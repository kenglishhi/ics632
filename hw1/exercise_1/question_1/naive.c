#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>


static float get_time_diff(struct timeval *s, struct timeval *e);

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


int main(int argc,char *argv[])
{

    if (argc < 2) {
	printf("Must supply option for Matrix Size, eg: naive.exe 1000 \n");
	return -1;
    }
    int debug = 0;
    if (argc == 3) {
	debug = 1;
    }
    int matrix_size = atoi(argv[1]);
    printf("Matrix_size is  %d\n", matrix_size);

    double a[matrix_size][matrix_size];
    double b[matrix_size][matrix_size];
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
	    a[i][j] = (double) rand();
	    b[i][j] = (double) rand();
//	    a[i][j] =  i + 1.0; 
//	    b[i][j] =  j + 1.0; 
	    result[i][j] = 0.0;
	}
    }

    printf("\nHere we go \n");
    /*
       // #1 : Do the MATRIX Multiplication i-j-k
     */
    gettimeofday(&start,NULL);

    char *implementation = "i-j-k";
    for (i = 0; i < matrix_size; i++) {
	for (j = 0; j < matrix_size; j++) {
	    for (k = 0; k < matrix_size; k++) {
		result[i][j] += a[i][k] * b[k][j];
		if (debug)
		    printf("\nresult[%d][%d] = %0.5f\n",i,j,result[i][j]);
	    }
	}
    }
    gettimeofday(&end,NULL);
    printf("Time difference for %s implementation %.5f seconds\n", implementation, get_time_diff(&start, &end));

    /*
       // #2 : Do the MATRIX Multiplication i-k-j
     */
    gettimeofday(&start,NULL);

    implementation = "i-k-j";
    for (i = 0; i < matrix_size; i++) {
	for (k = 0; k < matrix_size; k++) {
	    for (j = 0; j < matrix_size; j++) {
		result[i][j] += a[i][k] * b[k][j];
		if (debug)
		    printf("\nresult[%d][%d] = %0.5f\n",i,j,result[i][j]);
	    }
	}
    }
    gettimeofday(&end,NULL);
    printf("Time difference for %s implementation %.5f seconds\n", implementation, get_time_diff(&start, &end));

    /*
       // #3 : Do the MATRIX Multiplication j-i-k
     */
    gettimeofday(&start,NULL);

    implementation = "j-i-k";
    for (j = 0; j < matrix_size; j++) {
	for (i = 0; i < matrix_size; i++) {
	    for (k = 0; k < matrix_size; k++) {
		result[i][j] += a[i][k] * b[k][j];
		if (debug)
		    printf("\nresult[%d][%d] = %0.5f\n",i,j,result[i][j]);
	    }
	}
    }
    gettimeofday(&end,NULL);
    printf("Time difference for %s implementation %.5f seconds\n", implementation, get_time_diff(&start, &end));

    /*
       // #4 : Do the MATRIX Multiplication j-k-i
     */
    gettimeofday(&start,NULL);

    implementation = "j-k-i";
    for (j = 0; j < matrix_size; j++) {
	for (k = 0; k < matrix_size; k++) {
	    for (i = 0; i < matrix_size; i++) {
		result[i][j] += a[i][k] * b[k][j];
		if (debug)
		    printf("\nresult[%d][%d] = %0.5f\n",i,j,result[i][j]);
	    }
	}
    }
    gettimeofday(&end,NULL);
    printf("Time difference for %s implementation %.5f seconds\n", implementation, get_time_diff(&start, &end));

    /*
       // #5 : Do the MATRIX Multiplication k-i-j
     */
    gettimeofday(&start,NULL);

    implementation = "k-i-j";
    for (k = 0; k < matrix_size; k++) {
	for (i = 0; i < matrix_size; i++) {
	    for (j = 0; j < matrix_size; j++) {
		result[i][j] += a[i][k] * b[k][j];
		if (debug)
		    printf("\nresult[%d][%d] = %0.5f\n",i,j,result[i][j]);
	    }
	}
    }
    gettimeofday(&end,NULL);
    printf("Time difference for %s implementation %.5f seconds\n", implementation, get_time_diff(&start, &end));

    /*
       // #6 : Do the MATRIX Multiplication k-j-i
     */
    gettimeofday(&start,NULL);

    implementation = "k-j-i";
    int total_multiplications = 0;

    for (k = 0; k < matrix_size; k++) {
	for (j = 0; j < matrix_size; j++) {
	    for (i = 0; i < matrix_size; i++) {
		result[i][j] += a[i][k] * b[k][j];
                total_multiplications++; 
		if (debug)
		    printf("\nresult[%d][%d] = %0.5f\n",i,j,result[i][j]);
	    }
	}
    }
    gettimeofday(&end,NULL);
    printf("Time difference for %s implementation %.5f seconds\n", implementation, get_time_diff(&start, &end));

    gettimeofday(&start,NULL);
    multiply_matrices(&result[0][0], &a[0][0] , &b[0][0], matrix_size ) ; 
    gettimeofday(&end,NULL);
    printf("Time difference for optimal implementation %.5f seconds\n", get_time_diff(&start, &end));
    printf("Total Multiplications : %d\n", total_multiplications);


    return 1;
}
