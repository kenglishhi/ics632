#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>


float get_time_diff(struct timeval *s, struct timeval *e) ;

double mystery_function_800(double *x) ; 
int lgsearch(double step_size ) ; 

int main(int argc,char *argv[]) {
   srand(200000);
   if (argc < 3) { 
      printf("Usage: lgsearch.exe <number of trials> <strep size>\n");
      return -1;
   } 
   struct timeval section_start;
   struct timeval section_end;

   int trials = atoi(argv[1]);
   double step_size = strtod(argv[2], NULL);
   int i ;
   //float time_spent; 
   printf("trials,%d, step_size,%f\n",trials, step_size); 
   for (i = 0; i < trials; i++)  { 
      gettimeofday(&section_start,NULL);
      lgsearch(step_size); 
//      sleep(2);
      gettimeofday(&section_end,NULL);
      // time_spent = get_time_diff(&section_start, &section_end) ;  
      printf("%d,%f,%f\n", i, step_size, get_time_diff(&section_start, &section_end) + 0.1  ); 

   } 
   return 1;
}
