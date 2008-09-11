#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#define SIZE 800

static float getTimeDiff(struct timeval *s, struct timeval *e);

static float getTimeDiff(struct timeval *s, struct timeval *e)
{
	struct timeval	diff_tv;
		
	diff_tv.tv_usec = e->tv_usec - s->tv_usec;
	diff_tv.tv_sec = e->tv_sec - s->tv_sec;
		
	if (s->tv_usec > e->tv_usec)
	{
		diff_tv.tv_usec += 1000000;
		diff_tv.tv_sec--;
	}
		
	return (float) diff_tv.tv_sec + ((float) diff_tv.tv_usec / 1000000.0);
}


int main()
{
  struct timeval start;
  struct timeval end;
  double time_elapsed;
  int a[SIZE][SIZE]; 
  int b[SIZE][SIZE]; 
  int result[SIZE][SIZE]; 
  int i,k,j;
  gettimeofday(&start,NULL);


  // POPULATE the array
  for (i=0; i<SIZE; i++)  { 
    for (j=0; j<SIZE; j++){ 
      a[i][j] = i;
      b[i][j] = j;
      result[i][j] = 0; 
    } 
  } 

  printf("\nHello World\n");
  // Do the MATRIX Multiplication
  for (i=0; i<SIZE; i++) { 
    for (j=0; j<SIZE; j++) { 
      for (k=0; k<SIZE; k++) { 
        result[i][j] += a[i][k] * b[k][j];
      }
    } 
  }

//  sleep(4);

  gettimeofday(&end,NULL);

    printf("Time difference is %.5f seconds\n", getTimeDiff(&start, &end));


  return 1;
}
