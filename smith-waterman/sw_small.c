/* sw_test.c -- Test the speed of the standard Smith-Waterman algorithm for
 *
 *   comparison with bit-parallel constrained Smith-Waterman algorithm 
 *   http://coen.boisestate.edu/ssmith/BioHW/CompCode/BitParallel/sw_test.txt
 *
 */


#include  <stdio.h>
#define   STRLEN     8
#define   ITER       1
#define   GAP_OPEN   10
#define   GAP_CONT   2       


void print_matrix(long *matrix, int matrix_size) {
    long *current_value;
    int i,j;

    printf("matrix_size = %d \n\n", matrix_size  );
    for (i = 0; i < matrix_size; i++) {
        printf(" [" );
        for (j = 0; j < matrix_size; j++){
            current_value = matrix + i * matrix_size + j;
            printf(" %ld ", *current_value  );
        }
        printf("] " );

        printf("\n" );
    }
}


int main()
{
  long  i, j, d_loc, q_loc, d_index[STRLEN], q_index[STRLEN], Ins_c, Del_c, H_temp;
  char  aa[21] = "acdefghiklmnpqrstvwy";
  long  Ins[STRLEN+1][STRLEN+1], Del[STRLEN+1][STRLEN+1], H[STRLEN+1][STRLEN+1], Hg[STRLEN+1][STRLEN+1];

  /* Query Sequence = d1xpa_1 a.6.1.2 (134-210) from ASTRAL 1.67 40% */
  //char q[STRLEN] = "dkhklitkteakqeyllkdcdlekrepplkfivkknphhsqwgdmklylklqivkrslevwgsqealeeakevrqen";
  char q[STRLEN] = "ppeaiccc";
  /* Database Sequence =  d1jjcb2 a.6.1.1 (B:400-474) from ASTRAL 1.67 40% */
//  char d[STRLEN] = "ppeaipfrpeyanrllgtsypeaeqiailkrlgcrvegegptyrvtppshrldlrleedlveevariqgyetipl";
  char d[STRLEN] = "ppeaiccc";
  /* Scoring Matrix (PAM 250) */
  int  S[20][20] = {
  { 2, -2,  0,  0,  -4,   1,  -1,  -1,  -1,  -2,  -1,   0,   1,   0,  -2,   1,   1,   0,  -1,  -3},
  {-2, 12, -5, -5,  -4,  -3,  -3,  -2,  -5,  -6,  -5,  -4,  -3,  -5,  -4,   0,  -2,  -2,  -8,   0},
  { 0, -5,  4,  3,  -6,   1,   1,  -2,   0,  -4,  -3,   2,  -1,   2,  -1,   0,   0,  -2,  -7,  -4},
  { 0, -5,  3,  4,  -5,   0,   1,  -2,   0,  -3,  -2,   1,  -1,   2,  -1,   0,   0,  -2,  -7,  -4},
  {-4, -4, -6, -5,   9,  -5,  -2,   1,  -5,   2,   0,  -4,  -5,  -5,  -4,  -3,  -3,  -1,   0,   7},
  { 1, -3,  1,  0,  -5,   5,  -2,  -3,  -2,  -4,  -3,   0,  -1,  -1,  -3,   1,   0,  -1,  -7,  -5},
  {-1, -3,  1,  1,  -2,  -2,   6,  -2,   0,  -2,  -2,   2,   0,   3,   2,  -1,  -1,  -2,  -3,   0},
  {-1, -2, -2, -2,   1,  -3,  -2,   5,  -2,   2,   2,  -2,  -2,  -2,  -2,  -1,   0,   4,  -5,  -1},
  {-1, -5,  0,  0,  -5,  -2,   0,  -2,   5,  -3,   0,   1,  -1,   1,   3,   0,   0,  -2,  -3,  -4},
  {-2, -6, -4, -3,   2,  -4,  -2,   2,  -3,   6,   4,  -3,  -3,  -2,  -3,  -3,  -2,   2,  -2,  -1},
  {-1, -5, -3, -2,   0,  -3,  -2,   2,   0,   4,   6,  -2,  -2,  -1,   0,  -2,  -1,   2,  -4,  -2},
  { 0, -4,  2,  1,  -4,   0,   2,  -2,   1,  -3,  -2,   2,  -1,   1,   0,   1,   0,  -2,  -4,  -2},
  { 1, -3, -1, -1,  -5,  -1,   0,  -2,  -1,  -3,  -2,  -1,   6,   0,   0,   1,   0,  -1,  -6,  -5},
  { 0, -5,  2,  2,  -5,  -1,   3,  -2,   1,  -2,  -1,   1,   0,   4,   1,  -1,  -1,  -2,  -5,  -4},
  {-2, -4, -1, -1,  -4,  -3,   2,  -2,   3,  -3,   0,   0,   0,   1,   6,   0,  -1,  -2,   2,  -4},
  { 1,  0,  0,  0,  -3,   1,  -1,  -1,   0,  -3,  -2,   1,   1,  -1,   0,   2,   1,  -1,  -2,  -3},
  { 1, -2,  0,  0,  -3,   0,  -1,   0,   0,  -2,  -1,   0,   0,  -1,  -1,   1,   3,   0,  -5,  -3},
  { 0, -2, -2, -2,  -1,  -1,  -2,   4,  -2,   2,   2,  -2,  -1,  -2,  -2,  -1,   0,   4,  -6,  -2},
  {-6, -8, -7, -7,   0,  -7,  -3,  -5,  -3,  -2,  -4,  -4,  -6,  -5,   2,  -2,  -5,  -6,  17,   0},
  {-3,  0, -4, -4,   7,  -5,   0,  -1,  -4,  -1,  -2,  -2,  -5,  -4,  -4,  -3,  -3,  -2,   0,  10},
  };

  /* Convert residue characters to indices */
  printf("STRLEN = %d\n", STRLEN); 
  printf("d_loc = %d\n", STRLEN); 
  for (d_loc=0; d_loc<STRLEN; d_loc++){ 
    for (j=0; j<20; j++){ 
      if (d[d_loc] == aa[j]){ 
        d_index[d_loc] = j;
//        printf("d_index[%d] = %d\n", d_loc, d_index[d_loc]); 
      }  
    } 
  } 
  for (q_loc=0; q_loc<STRLEN; q_loc++) { 
    for (j=0; j<20; j++) { 
      if (q[q_loc] == aa[j]) { 
        q_index[q_loc] = j;
//        printf("q_index[%d] = %d\n", q_loc, q_index[q_loc]); 
      } 
    } 
  }
  printf("d_loc = %d\n", STRLEN); 
  for (i=0; i<STRLEN+1; i++) {
    for (j=0; j<STRLEN+1; j++) {
      Ins[i][j] = 0;
      Del[i][j] = 0;
      H[i][j] = 0;
      Hg[i][j] = - GAP_OPEN;
    } 
  } 


  /* Iterate calculation enough times to get good timing results */
  for (i=0; i<ITER; i++) {
    printf("iteration = %ld\n", i); 

    /* Intialize Ins, Del, and H */
    for (d_loc=0; d_loc<STRLEN+1; d_loc++) {
      Ins[d_loc][0] = 0;
      Del[d_loc][0] = 0;
      H[d_loc][0] = 0;
      Hg[d_loc][0] = - GAP_OPEN;
    }
    for (q_loc=1; q_loc<STRLEN+1; q_loc++) {
      Ins[0][q_loc] = 0;
      Del[0][q_loc] = 0;
      H[0][q_loc] = 0;
      Hg[0][q_loc] = - GAP_OPEN;
    }      

    /* Do scoring */
    for (d_loc=0; d_loc<STRLEN; d_loc++) { 
      for (q_loc=0; q_loc<STRLEN; q_loc++) {
//        printf("score[%d][%d] \n", d_loc, q_loc); 
        Ins_c = Ins[d_loc][q_loc+1] - GAP_CONT;
        if (Hg[d_loc][q_loc+1] > Ins_c)
          Ins[d_loc+1][q_loc+1] = Hg[d_loc][q_loc+1];
        else
          Ins[d_loc+1][q_loc+1] = Ins_c;
        Del_c = Del[d_loc+1][q_loc] - GAP_CONT;
        if (Hg[d_loc+1][q_loc] > Del_c) { 
          Del[d_loc+1][q_loc+1] = Hg[d_loc+1][q_loc];
        } else { 
          Ins[d_loc+1][q_loc+1] = Del_c;
        } 
        if (Ins[d_loc][q_loc] > Del[d_loc][q_loc])
          H_temp = Ins[d_loc][q_loc];
        else
          H_temp = Del[d_loc][q_loc];
        if (H_temp < H[d_loc][q_loc]) { 
          H_temp = H[d_loc][q_loc];
        } 
        H[d_loc+1][q_loc+1] = H_temp + S[q_index[q_loc]][d_index[d_loc]];
        printf("H_temp = %ld \n", H_temp); 
        printf("H[%ld][%ld] = %ld \n", d_loc+1, q_loc+1, H[d_loc+1][q_loc+1] ); 
        if (H[d_loc+1][q_loc+1] < 0)
          H[d_loc+1][q_loc+1] = 0;
        Hg[d_loc+1][q_loc+1] = H[d_loc+1][q_loc+1] - GAP_OPEN;

//      printf("Hg[%d][%d] = %d \n", d_loc+1, q_loc+1, Hg[d_loc+1][q_loc+1] ); 
//      printf("Hg[%d][%d] = %d \n", d_loc+1, q_loc+1, Hg[d_loc+1][q_loc+1] ); 
      }
    } 
    printf("Hg == \n"); 
    print_matrix(Hg, STRLEN+1); 
    printf("Ins == \n"); 
    print_matrix(Ins, STRLEN+1); 
    printf("Del == \n"); 
    print_matrix(&Del, STRLEN+1); 
  }
  return 1;
}

