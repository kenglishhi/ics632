main: clean
	gcc -O0 naive.c -o ~/bin/naive.exe
	gcc -O0 -Wall matrix_multiply_best_order.c -o ~/bin/matrix_multiply_best_order.exe
	gcc -O0 -Wall  matrix_multiply_i_j_k.c -o ~/bin/matrix_multiply_i_j_k.exe
	gcc -O0 -Wall matrix_multiply_with_blocks.c -o ~/bin/matrix_multiply_with_blocks.exe
	gcc -O0 optimized_matrix_multiply.c -o ~/bin/optimized_matrix_multiply.exe
	cp simple_job_script.sh ~/bin/
	chmod u+x simple_job_script.sh
optimized: 
	gcc -O3 -Wall matrix_multiply_best_order.c -o ~/bin/matrix_multiply_best_order.exe
	gcc -O3 -Wall matrix_multiply_i_j_k.c -o ~/bin/matrix_multiply_i_j_k.exe
	gcc -O3 optimized_matrix_multiply.c -o ~/bin/optimized_matrix_multiply.exe
clean:
	/bin/rm -f ~/bin/naive.exe  ~/bin/matrix_multiply_with_blocks.exe ~/bin/optimized_matrix_multiply.exe
