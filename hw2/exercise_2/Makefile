

GCC = gcc 


main: clean timediff lgsearch
	$(GCC) -Wall  lgsearch1.c  mystery_function_800_v2.o ../lib/timediff.o lgsearch.o  -lm -o ~/bin/lgsearch1.exe 
clean:
	/bin/rm -f ~/bin/lgsearch*.exe  ../lib/timediff.o lgsearch.o  
timediff:
	$(GCC) -c ../lib/timediff.c  -o ../lib/timediff.o
lgsearch:
	$(GCC) -c lgsearch.c
