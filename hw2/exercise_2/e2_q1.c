#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

double mystery_function_800(double *);

double local_search(double step_size, int trial) {
  double x[800];
  int my_seed = getpid() * trial;
  int i;
  double current_value, new_value;
  char no_improvement = 1;

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


int main(int argc, char **argv) {
  int num_trials;
  double step_size;
  double best_minimum;
  struct timeval start, finish;
  int i;

  if (argc != 3) {
    fprintf(stderr,"Usage: %d <num trials> <step size>\n",argc);
    exit(1); 
  }

  if ((sscanf(argv[1],"%d",&num_trials) != 1) ||
      (sscanf(argv[2],"%lf",&step_size) != 1)) {
    fprintf(stderr,"Usage: %d <num trials> <step size>\n");
    exit(1); 
  }


  gettimeofday(&start,NULL);

  best_minimum = -1.0;
  for (i=0; i<num_trials; i++) {
    double local_minimum;

    local_minimum = local_search(step_size, i);
    //fprintf(stdout,"Local minimum found for trial #%d: %.4f\n",i,local_minimum); 
    if ((best_minimum < 0) || (local_minimum < best_minimum)) {
      best_minimum = local_minimum;
    }
  }

  gettimeofday(&finish,NULL);

  fprintf(stdout,"Time to solution: %.4f\n",
     ((finish.tv_sec*1000000.0+finish.tv_usec) - (start.tv_sec*1000000.0+start.tv_usec)) / 1000000.00);

	
  fprintf(stdout,"Best found minimum: %.4f\n",best_minimum);
}
