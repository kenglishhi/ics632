main: clean  ring_matrix_multiply
	mpicc -Wall test_send_receive.c ring_matrix_multiply.o -o test_send_receive
	mpicc -Wall q1.c ring_matrix_multiply.o  -o q1
	mpicc -Wall q2_L_b.c ring_matrix_multiply.o -o q2_L_b
	mpicc -Wall q2_w.c ring_matrix_multiply.o -o q2_w
clean:
	/bin/rm -f q1 q2_L_b *.o test_send_receive
ring_matrix_multiply:
	mpicc -c -Wall ring_matrix_multiply.c -o ring_matrix_multiply.o 
