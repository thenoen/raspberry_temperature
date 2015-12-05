all:
	gcc -o scparser scparser.c

clean:
	rm -rf scparser
	rm -rf test

build_test:
	gcc -o test test.c scparser.c
