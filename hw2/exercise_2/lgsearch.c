#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define ARRAY_SIZE 800
//float get_time_diff(struct timeval *s, struct timeval *e) ;

double lgsearch(double step_size, int trial ) {
  double x[800];
  int my_seed = getpid() * trial;
  int i;
  double current_value, new_value;

  for (i=0; i<800; i++) {
    x[i] = 50.0 * (double)rand_r(&my_seed)/(double)RAND_MAX;
  }

  for (i=0; i<800; i++) {
    current_value = mystery_function_800(x);
    while (1) {
      if (x[i] + step_size > 49.9)
        break;
      x[i] += step_size;
      new_value = mystery_function_800(x);
      if (new_value > current_value) {
        x[i] -= step_size;
        break;
      } else {
        current_value = new_value;
      }
    }
  }
  return current_value;
}


