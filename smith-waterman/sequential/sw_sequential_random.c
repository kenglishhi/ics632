/*************************************
 *
 * sw_sequential_random.c
 * Author: Kevin English
 * University of Hawaii
 * ICS 632
 *
 * This program takes a length and from that generates 2 random strings. 
 * It then runs the Smith-Waterman algorithm on them and determines the optimal local alignment.
 * An extra flag may be sent to program so that it generates one random string and then runs SW on itself.
 * This is useful to find the worst case of the backtrace algorithm.
 **/


#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define DEBUG   0
#define STRLEN 8
#define STRLEN 8
#define ITER   1
#define GAP   -1
#define MATCH  1
#define MISMATCH -1
#define SEQ1_LEN  8
#define DIRECTION_NONE     0
#define DIRECTION_UP       1
#define DIRECTION_LEFT     2
#define DIRECTION_DIAGONAL 3

void generate_random_array(int *, int, int ) ;
double get_time_diff(struct timeval *start, struct timeval *finish) ;
void print_score_matrix(int *, int, int ) ;
int do_sw(int *,int, int *,int, int *, int *, int *,int *) ;

int main(int argc,char *argv[]) {

  /* Parse Command Line Args */
  int ncols, chunk_size ;
  char *program_name = "sw_sequential_random" ;

  if (argc < 3) {
    printf("[%s] You need 2 arguments\n", program_name);
    return 0 ;
  } else {
    if ((sscanf(argv[1],"%d",&ncols) != 1) ||
        (sscanf(argv[2],"%d",&chunk_size) != 1)) {
      fprintf(stderr,"Usage: %s <ncols> <chunk_size>\n", argv[0] );
      exit(1);
    }
  }
  int seq1_length = ncols - 1;
  int seq2_length = seq1_length ;

  srand(time(0)) ;

  struct timeval total_start;
  struct timeval total_finish;

  gettimeofday(&total_start,NULL);


  char alphabet[20] = "acdefghiklmnpqrstvwy";
  char *seq1, *seq2  ;
  int *seq1_arr, *seq2_arr  ;
  int *align1_arr, *align2_arr  ;
  int i;

//  arr = (int *) calloc(size,sizeof(int) )  ;

  seq1_arr = (int *) malloc(seq1_length * sizeof(int) )  ;
  seq2_arr = (int *) malloc(seq2_length * sizeof(int) )  ;

  seq1 = (char *) malloc(seq1_length * sizeof(char) )  ;
  seq2 = (char *) malloc(seq2_length * sizeof(char) )  ;

  align1_arr = (int *) malloc(seq1_length * sizeof(int) )  ;
  align2_arr = (int *) malloc(seq2_length * sizeof(int) )  ;

  // generate a random array
  generate_random_array(seq1_arr, seq1_length,  20);

  // an extra flag may be sent 
  if (argc == 4)
    seq2_arr = seq1_arr;
  else
    generate_random_array(seq2_arr, seq2_length,  20);

  printf("%s, seq1_length  = %d\n", program_name, seq1_length);
  printf("%s, seq2_length  = %d\n", program_name, seq2_length);

  for(i=0; i < seq1_length; i++){
    seq1[i] =  alphabet[seq1_arr[i]] ;
  }
  printf("%s, Alphabet 1\n", program_name);
  for(i=0; i < seq2_length; i++){
    seq2[i] =  alphabet[seq2_arr[i]] ;
  }

  if (DEBUG) {
    printf("%s, seq1 = %s\n", program_name, seq1);
    printf("%s, seq2 = %s\n", program_name, seq2);
  }

  int align1_length,align2_length;


  do_sw(seq1_arr, seq1_length, seq2_arr, seq2_length,
        align1_arr, &align1_length, align2_arr, &align2_length );

  gettimeofday(&total_finish,NULL);

  printf("%s, Time, Columns\n", program_name) ;
  printf("%s, %f, %d,result \n", program_name, get_time_diff(&total_start, &total_finish), ncols) ;

  return 1;
}

