#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

#define ARRAY_SIZE 800
float get_time_diff(struct timeval *s, struct timeval *e) ;

int lgsearch(double step_size ) {
    double x[ARRAY_SIZE];
    int i;
    struct timeval start;
    struct timeval end;


    for (i=0; i < ARRAY_SIZE ; i++ ) {
	x[i]  = 49.9  ;//  (double) (50.0*rand()/(RAND_MAX+1.0));
        printf("x[i] = %f\n", x[i]) ; 
    }

    double *item;
    *item = x[0]; 
    printf("item  = %f\n", *item ) ; 
    
    gettimeofday(&start,NULL);

    double prev_result = 0.0;
    double cur_result = 0.0;
    i = 0;
    int reset = 1;
    double threshold = 50.0; 
    do {
	if (reset) {
	    prev_result = mystery_function_800(&x[0]);
	    reset = 0;
	}
        printf( " i=%d-- old value = %f, %f, %d \n", i, *item, (*item + step_size),  ((*item + step_size) < threshold ) ) ; 

        if (! ((*item + step_size) < 50.0 )) { 
            printf(" too big!")  ;
	    item +=  1 ;
            printf(" new item, %f !", *item )  ;
            i++;
	    continue ;
        } 
	*item +=  step_size;
        printf( " i=%d-- new value = %.5f \n", i, *item) ; 
	cur_result = mystery_function_800(&x[0]);

	if ((cur_result - prev_result) > 0.0 ) {
	    prev_result = cur_result;
	}  else {
	    x[i] -= step_size;
	    i++;
	    reset= 1;
	    printf(".");
	    fflush(stdout);
	}
    } while (i < 10) ;
//    } while (i < ARRAY_SIZE) ;
    printf("\n");
    gettimeofday(&end,NULL);
    printf("Time Spent %f seconds\n", get_time_diff(&start, &end));

    return 1;
}


