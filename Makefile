all:test
test:test.c
	gcc -o test test.c
run:
	./test
clean:
	rm -r test	
