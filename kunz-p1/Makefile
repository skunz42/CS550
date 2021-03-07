all:	shell

shell:	parser.o jobs.o pipes.o
	gcc parser.o jobs.o pipes.o -o shell

parser.o:	parser.c
	gcc -g -c -Wall parser.c

jobs.o:	jobs.c
	gcc -g -c -Wall jobs.c

pipes.o:	pipes.c
	gcc -g -c -Wall pipes.c

clean:
	rm -f *.o shell
