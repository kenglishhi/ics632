#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>


float get_time_diff(struct timeval *s, struct timeval *e) ;

double mystery_function_800(double *x) ;
int lgsearch(double step_size, int trials ) ;

int main(int argc,char *argv[]) {
    srand(200000);
    if (argc < 3) {
	printf("Usage: lgsearch.exe <number of trials> <strep size>\n");
	return -1;
    }
    struct timeval start;
    struct timeval finish;

    int trials = atoi(argv[1]);
    double step_size = strtod(argv[2], NULL);
    int i ;
    //float time_spent;
    printf("trials,%d, step_size,%f\n",trials, step_size);

    gettimeofday(&start,NULL);

    double best_minimum = -1.0;
    for (i=0; i < trials; i++) {
       double local_minimum;

       local_minimum = lgsearch(step_size, i);
       //fprintf(stdout,"Local minimum found for trial #%d: %.4f\n",i,local_minimum); 
       if ((best_minimum < 0) || (local_minimum < best_minimum)) {
         best_minimum = local_minimum;
       }
     }
  gettimeofday(&finish,NULL);
  fprintf(stdout,"Time to solution: %.4f\n",
     ((finish.tv_sec*1000000.0+finish.tv_usec) - (start.tv_sec*1000000.0+start.tv_usec)) / 1000000.00);


  fprintf(stdout,"Best found minimum: %.4f\n",best_minimum);


    return 1;
}
