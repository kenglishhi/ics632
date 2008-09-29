#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>


void omp_set_num_threads(int number);
float get_time_diff(struct timeval *s, struct timeval *e) ;

double mystery_function_800(double *x) ;
int lgsearch(double step_size ) ;

int main(int argc,char *argv[]) {
    srand(200000);
    if (argc < 3) {
	printf("Usage: lgsearch2.exe <number of trials> <strep size>\n");
	return -1;
    }
    int trials = atoi(argv[1]);
    double step_size = strtod(argv[2], NULL);

    printf("CALLED lgsearch2.exe %d %.5f\n\n", trials, step_size);
    omp_set_num_threads(2);
    int i;
    omp_set_dynamic() ;

    #pragma omp parallel shared(step_size,trials) private (i)
    {
	#pragma omp for schedule(dynamic)
	for (i=0; i < trials; i++) {
	    printf("Trial %d\n", i);
	    struct timeval section_start;
	    struct timeval section_end;
	    gettimeofday(&section_start, NULL);
	    lgsearch(step_size);
	    gettimeofday(&section_end, NULL);
	    float section_time_spent = get_time_diff(&section_start, &section_end) ;
	    printf("\nTrial %d took %f seconds\n", i, section_time_spent );
	}
    }


    return 1;
}
