main: clean timediff
	mpicc -Wall send_receive.c test_send_receive.c ../lib/timediff.o -o test_send_receive.exe
	mpicc -Wall send_receive.c ring_matrix_multiply.c ../lib/timediff.o -o ring_matrix_multiply.exe
timediff:
	gcc -c ../lib/timediff.c  -o ../lib/timediff.o
clean:
	/bin/rm -f *.exe *.o ../lib/*.o
