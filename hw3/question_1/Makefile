main: clean
	mpicc -Wall send_receive.c test_send_receive.c -o test_send_receive.exe
	mpicc -Wall send_receive.c ring_matrix_multiply.c -o ring_matrix_multiply.exe
clean:
	/bin/rm -f *.exe *.o
