main: clean timediff
	mpicc -Wall send_receive.c test_send_receive.c timediff.o -o test_send_receive.exe
	mpicc -Wall send_receive.c ring_matrix_multiply.c timediff.o -o ring_matrix_multiply.exe
	mpicc -Wall send_receive.c ring_matrix_multiply_perform.c timediff.o -o ring_matrix_multiply_perform.exe
timediff:
	gcc -c timediff.c  -o timediff.o
clean:
	/bin/rm -f *.exe *.o ../lib/*.o
