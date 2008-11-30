/* sw_sequential.c */

#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>


#define STRLEN     8
#define ITER       1
#define DEBUG       0
#define GAP   -1
#define MATCH 1
#define MISMATCH -1
#define SEQ1_LEN 8
#define DIRECTION_NONE     0
#define DIRECTION_UP       1
#define DIRECTION_LEFT     2
#define DIRECTION_DIAGONAL 3

int do_sw(int *, int , int *, int , int *, int *, int *, int *); 
int  main(int argc,char *argv[]) {
  char  alphabet[21] = "acdefghiklmnpqrstvwy";
  char *seq1;
  char *seq2;
  if (argc < 3) {
     printf("You need 2 arguments\n");
     return 0 ;
  } else {
     seq1 = argv[1] ;
     seq2 = argv[2] ;
  }

  printf("seq1 = %s\n", seq1);
  printf("seq2 = %s\n", seq2);

  int seq1_length = 0 ;
  int seq2_length = 0;
  while (seq1[seq1_length] != '\0') {
    seq1_length++;
  }
  printf("seq1_length = %d\n", seq1_length );
  while (seq2[seq2_length] != '\0') {
    seq2_length++;
  }
  printf("seq2_length = %d\n", seq2_length );

  int seq1_arr[seq1_length], seq2_arr[seq2_length]  ;
  int align1_arr[seq1_length], align2_arr[seq2_length]  ;
  int align1_length = 0, align2_length = 0  ;
  int i, j;

  /* Convert characters to indices so we are only doing integer comparisons...*/
  printf("STRLEN = %d\n", STRLEN);
  printf("d_loc = %d\n", STRLEN);
  printf("seq1 = %s\n", seq1);
  printf("seq2 = %s\n", seq2);
  for (i=0; i < seq1_length; i++)
    for (j=0; j<20; j++)
      if (seq1[i] == alphabet[j])
        seq1_arr[i] = j;

  for (i=0; i < seq2_length; i++)
    for (j=0; j<20; j++)
      if (seq2[i] == alphabet[j])
        seq2_arr[i] = j;

  do_sw(&seq1_arr[0],  seq1_length, &seq2_arr[0], seq2_length, &align1_arr[0], &align1_length, &align2_arr[0], &align2_length);  
  printf("align1_length = %d , align2_length = %d\n ", align1_length, align2_length ) ;

  char *align1, *align2  ;
  align1 = (char *) calloc(seq1_length,sizeof(char) )  ;
  align2 = (char *) calloc(seq2_length,sizeof(char) )  ;
  printf("align1_length = %d , align2_length = %d\n ", align1_length, align2_length ) ;
  for(i=0;i<align1_length;i++){
     
    align1[align1_length-i-1] =  alphabet[align1_arr[i]] ;
  }
  for(i=0;i<align2_length;i++){
    align2[align2_length-i-1] =  alphabet[align2_arr[i]] ;
  }

  printf("align1 = %s\n", align1);
  printf("align2 = %s\n", align1);

  return 0; 
}
