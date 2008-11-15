/* sw_sequential.c */

#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

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

void generate_random_array(int *, int , int ) ; 
double get_time_diff(struct timeval *start, struct timeval *finish) ;
void print_score_matrix(int *, int , int ) ; 
int do_sw(int *,int, int *,int, int *, int *, int *,int *) ; 

int main(int argc,char *argv[]) { 
  
  int seq1_length, seq2_length ; 
  if (argc < 3) {
     printf("You need 2 arguments\n");
     return 0 ;
  } else {

    if ((sscanf(argv[1],"%d",&seq1_length) != 1) ||
        (sscanf(argv[2],"%d",&seq2_length) != 1)) {
      fprintf(stderr,"Usage: %s <num trials> <step size>\n", argv[0] );
      exit(1);
    }

  }
  srand(time(0)) ;  


  char  alphabet[20] = "acdefghiklmnpqrstvwy"; 
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

  generate_random_array(seq1_arr, seq1_length,  20);   
  generate_random_array(seq2_arr, seq2_length,  20);   

  printf("seq1_length  = %d\n", seq1_length); 
  printf("seq2_length  = %d\n", seq2_length); 

  printf("alphabet[%d] = %c \n", 20, alphabet[20]  ); 

  for(i=0;i < seq1_length;i++){ 
    seq1[i] =  alphabet[seq1_arr[i]] ; 
  } 
  for(i=0;i < seq2_length; i++){ 
    seq2[i] =  alphabet[seq2_arr[i]] ; 
  } 

  if (DEBUG) { 
    printf("seq1 = %s\n", seq1); 
    printf("seq2 = %s\n", seq2); 
  } 



//  char  alphabet[21] = "acdefghiklmnpqrstvwy"; 
  int align1_length,align2_length;

  struct timeval total_start;
  struct timeval total_finish;

  gettimeofday(&total_start,NULL); 
  do_sw(seq1_arr, seq1_length, seq2_arr, seq2_length, 
        align1_arr, &align1_length, align2_arr, &align2_length ); 

  gettimeofday(&total_finish,NULL); 
  printf("%.8f nanosecs per unit work\n", get_time_diff(&total_start, &total_finish) *1000000.0) ; 
  char *align1, *align2  ; 
  align1 = (char *) calloc(seq1_length,sizeof(char) )  ; 
  align2 = (char *) calloc(seq2_length,sizeof(char) )  ; 

  for(i=0;i<align1_length;i++){ 
    align1[align1_length-i-1] =  alphabet[align1_arr[i]] ; 
  } 
  for(i=0;i<align2_length;i++){ 
    align2[align2_length-i-1] =  alphabet[align2_arr[i]] ; 
  } 

  printf("align1 = %s\n", align1); 
  printf("align2 = %s\n", align1); 


/*
$align1 = reverse $align1;
$align2 = reverse $align2;
print "$align1\n";
print "$align2\n"

*/
return 1; 
}

