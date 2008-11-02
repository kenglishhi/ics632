/* sw_simple.c -- A simple implementation of the sw
 *
 *   comparison with bit-parallel constrained Smith-Waterman algorithm 
 *   http://coen.boisestate.edu/ssmith/BioHW/CompCode/BitParallel/sw_test.txt
 *
 */


#include  <stdio.h>
#define   STRLEN     5
#define   RESULT_LEN     6
#define   ITER       1
#define   GAP_OPEN   10
#define   GAP_CONT   2       

int main()
{

  char q[STRLEN] = "HEAGA";
  char d[STRLEN] = "PAWHA";
  char  aa[21] = "ACDEFGHIKLMNPQRSTVWY";

  int q_score_matrix[STRLEN] = {-1, -1, -1, -1, -1} ; 
  int d_score_matrix[STRLEN] = {-1, -1, -1, -1, -1} ; 

  int i, j, d_loc, q_loc, d_index[STRLEN], q_index[STRLEN] ; 
  int d_len = STRLEN; 
  int q_len = STRLEN; 
  int result[RESULT_LEN][RESULT_LEN] ; 

  for (i=0; i < RESULT_LEN ; i++) { 
    for (j=0; j< RESULT_LEN ; j++) { 
       if (i== 0 && j==0 ) { 
          result[i][j] = 0; 
       } else if (i ==0){ 
          result[i][j] = -j; 
       } else if (j ==0){ 
          result[i][j] = -i; 
       } else { 
          result[i][j] = 0; 
       } 

       printf("result[%d][%d] = %d\n", i, j, result[i][j] ); 
    } 
  } 
  


  /* Convert residue characters to indices */
  for (d_loc=0; d_loc < d_len; d_loc++) {
    for (j=0; j<20; j++) {
      if (d[d_loc] == aa[j]) {
        d_index[d_loc] = j;
      }
    }
  }
  for (q_loc=0; q_loc < q_len; q_loc++) {
    for (j=0; j<20; j++) {
      if (q[q_loc] == aa[j]) {
        q_index[q_loc] = j;
      }
    }

  }
  
  for (i=1; i < RESULT_LEN; i++) {
    for (j=1; j < RESULT_LEN; j++) {
       printf("result[%d][%d] = %d\n", i, j, result[i][j] ); 
    } 
  }


  
 

  return 0; 


}

