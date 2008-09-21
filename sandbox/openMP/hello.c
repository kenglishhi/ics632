#include <omp.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

  void omp_set_num_threads(int number);

int main() { 

   omp_set_num_threads(15);


  printf("BEGIN------\n");
  #pragma omp parallel
  {
      printf("Hello World\n"); 
  } 
  printf("Done\n");  
}
