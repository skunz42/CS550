all:	shell

shell:	parser.o jobs.o
	gcc parser.o jobs.o -o shell

parser.o:	parser.c
	gcc -g -c -Wall parser.c

jobs.o:	jobs.c
	gcc -g -c -Wall jobs.c

clean:
	rm -f *.o shell
