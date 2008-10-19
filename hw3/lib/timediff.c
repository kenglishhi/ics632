#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

double get_time_diff(struct timeval *start, struct timeval *finish) {
   return (((finish->tv_sec*1000000.0+finish->tv_usec) - (start->tv_sec*1000000.0+start->tv_usec)) / 1000000.00);
/*    struct timeval diff_tv;

    diff_tv.tv_usec = e->tv_usec - s->tv_usec;
    diff_tv.tv_sec = e->tv_sec - s->tv_sec;

    if (s->tv_usec > e->tv_usec) {
        diff_tv.tv_usec += 1000000;
        diff_tv.tv_sec--;
    }
    return (float) diff_tv.tv_sec + ((float) diff_tv.tv_usec / 1000000.0);
*/
}



