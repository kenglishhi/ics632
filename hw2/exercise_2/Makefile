

GCC = mpicc 


main: clean timediff lgsearch
	$(GCC) -Wall  lgsearch1.c  mystery_function_800_v2.o ../lib/timediff.o lgsearch.o -lm  -o ~/bin/lgsearch1.exe 
	$(GCC) -Wall  lgsearch_para.c  mystery_function_800_v2.o ../lib/timediff.o lgsearch.o -lm  -o lgsearch_para.exe 
clean:
	/bin/rm -f lgsearch*exe  ../lib/timediff.o lgsearch.o  lgsearch*.exe
timediff:
	$(GCC) -c ../lib/timediff.c  -o ../lib/timediff.o
lgsearch:
	$(GCC) -c lgsearch.c
