all:fibo subsystem
fibo:./code/fibo.c ./code/main.c
	gcc -o fibo ./code/fibo.c ./code/main.c -I ./include
subsystem:
	cd code && $(MAKE)
clean:
	rm -f ./code/fibo.o
	rm -f ./code/main.o

