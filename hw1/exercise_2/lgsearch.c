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
	x[i]  = (double) (50.0*rand()/(RAND_MAX+1.0));
    }

    double *item;
    item = &x[0]; 
    
    gettimeofday(&start,NULL);

    double prev_result = 0.0;
    double cur_result = 0.0;
    i = 0;
    int reset = 1;
    int var; 
    double threshold = 50.0; 
    double next_value = 50.0; 
    double difference;
    int cur, prev;
    do {
        next_value = *item + step_size; 
        if (next_value >= threshold ) { 
           // if we are already at the past the threshold, go to the next item in the loop 
           item +=  1 ;
           i++;
           reset= 1;
           continue;   
        }         

	if (reset) {
	    prev_result = mystery_function_800(&x[0]);
	    reset = 0;
	}

	*item =  next_value;
	cur_result = mystery_function_800(&x[0]);
	if (cur_result > prev_result ) {
	    prev_result = cur_result;
	}  else {
	    item += 1 ; 
	    i++;
	    reset= 1;
	}
    } while (i < ARRAY_SIZE) ;
    gettimeofday(&end,NULL);
    printf("Time Spent %f seconds\n", get_time_diff(&start, &end));

    return 1;
}


