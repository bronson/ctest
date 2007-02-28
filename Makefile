mutest: mutest.c main.c mutest.h Makefile
	gcc -Wall -Werror -g mutest.c main.c -DMUTEST_MAIN -o mutest