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

    struct timeval section1_start;
    struct timeval section1_end;

    struct timeval section2_start;
    struct timeval section2_end;


    printf("CALLED lgsearch2.exe %d %.5f\n\n", trials, step_size);
    omp_set_num_threads(2);
    #pragma omp parallel
    {
	#pragma omp sections
	{
	    #pragma omp section
	    {
		gettimeofday(&section1_start,NULL);

		printf("New Thread, doing trials 0 to %d\n", trials/2 );
		int i ;
		for (i = 0; i < trials/2; i++)  {
		    printf("trial %d\n", i) ;
		    lgsearch(step_size);
		}
		gettimeofday(&section1_end,NULL);
	    }
	    #pragma omp section
	    {
		gettimeofday(&section2_start,NULL);
		printf("New Thread, doing trials  to %d to %d \n", trials/2, trials );
		int i ;
		for (i = trials/2; i < trials; i++)  {
		    printf("trial %d\n", i) ;
		    lgsearch(step_size);
		}
		gettimeofday(&section2_end,NULL);
	    }
	}
    }
    float section1_time_spent = get_time_diff(&section1_start, &section1_end) ;  
    float section2_time_spent = get_time_diff(&section2_start, &section2_end) ; 
    
    printf("Section 1 Time Spent %.5f seconds\n", section1_time_spent );
    printf("Section 2 Time Spent %.5f seconds\n", section2_time_spent ); 
    float load_imbalance ; 
    int faster_section ; 
    if (section2_time_spent < section1_time_spent ) { 
        faster_section  = 2; 
        load_imbalance = (section1_time_spent - section2_time_spent ) /   section2_time_spent ; 
    }  else {
        faster_section  = 1; 
        load_imbalance = (section2_time_spent - section1_time_spent ) /   section1_time_spent ; 
    } 
    printf("Load imbalance: %.5f \n", load_imbalance ); 
    printf("Faster Section %d \n", faster_section ); 

    return 1;
}
