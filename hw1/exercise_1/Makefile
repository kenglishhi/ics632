main: clean
	gcc -O0 naive.c -o ~/bin/naive.exe
	gcc  -O0 -Wall matrix_multiply_with_blocks.c -o ~/bin/matrix_multiply_with_blocks.exe
	gcc -O0 optimized_matrix_multiply.c -o ~/bin/optimized_matrix_multiply.exe
optimized: 
	gcc -O3 naive.c -o ~/bin/naive.exe
	gcc  -O3 -Wall matrix_multiply_with_blocks.c -o ~/bin/matrix_multiply_with_blocks.exe
	gcc -O3 optimized_matrix_multiply.c -o ~/bin/optimized_matrix_multiply.exe
clean:
	/bin/rm -f ~/bin/naive.exe  ~/bin/matrix_multiply_with_blocks.exe ~/bin/optimized_matrix_multiply.exe
