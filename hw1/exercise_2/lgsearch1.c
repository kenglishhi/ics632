#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>


double mystery_function_800(double *x) ; 
int lgsearch(double step_size ) ; 

int main(int argc,char *argv[]) {
   srand(200000);
   if (argc < 3) { 
      printf("Usage: lgsearch.exe <number of trials> <strep size>\n");
      return -1;
   } 
   int trials = atoi(argv[1]);
   double step_size = strtod(argv[2], NULL);
   printf("CALLED lgsearch1.exe %d %.5f\n\n" , trials, step_size);
   int i ;
   for (i = 0; i < trials; i++)  { 
      lgsearch(step_size); 
   } 
   return 1;
}
