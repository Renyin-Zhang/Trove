# A Makefile to build 'trove' project

C11     =  cc -std=c11
CFLAGS  =  -Wall -Werror

trove: build.c list.c main.c remove.c
	$(C11) $(CFLAGS) build.c list.c main.c remove.c -o trove
clean: 
	rm -rf trove *.o
