main: clean 
	mpicc -Wall ring_matrix_multiply.c test_send_receive.c -o test_send_receive.exe
	mpicc -Wall ring_matrix_multiply.c q1.c -o q1.exe
	mpicc -Wall q2.c ring_matrix_multiply.c -o q2.exe
clean:
	/bin/rm -f *.exe *.o 
