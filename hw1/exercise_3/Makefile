main: clean timediff 
	gcc -Wall jacobi.c  ../lib/timediff.o  -lm -o ~/bin/jacobi.exe 
clean:
	/bin/rm -f ~/bin/jacobi*.exe  ../lib/timediff.o  
timediff:
	gcc -c ../lib/timediff.c  -o ../lib/timediff.o
