
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

float get_time_diff(struct timeval *s, struct timeval *e) {
    struct timeval diff_tv;

    diff_tv.tv_usec = e->tv_usec - s->tv_usec;
    diff_tv.tv_sec = e->tv_sec - s->tv_sec;

    if (s->tv_usec > e->tv_usec) {
        diff_tv.tv_usec += 1000000;
        diff_tv.tv_sec--;
    }
    return (float) diff_tv.tv_sec + ((float) diff_tv.tv_usec / 1000000.0);
}



