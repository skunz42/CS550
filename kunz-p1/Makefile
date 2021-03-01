all:	shell inf

shell:	parser.o jobs.o
	gcc parser.o jobs.o -o shell

inf:	inf.o
	gcc inf.o -o inf

parser.o:	parser.c
	gcc -g -c -Wall parser.c

jobs.o:	jobs.c
	gcc -g -c -Wall jobs.c

inf.o:	inf.c
	gcc -g -c -Wall inf.c

clean:
	rm -f *.o shell inf
