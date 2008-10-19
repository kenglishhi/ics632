main: clean timediff
	mpicc -Wall ring_matrix_multiply.c test_send_receive.c -o test_send_receive.exe
	mpicc -Wall ring_matrix_multiply.c q1.c -o q1.exe
	mpicc -Wall send_receive.c ring_matrix_multiply_perform.c timediff.o -o ring_matrix_multiply_perform.exe
timediff:
	gcc -c timediff.c  -o timediff.o
clean:
	/bin/rm -f *.exe *.o 
