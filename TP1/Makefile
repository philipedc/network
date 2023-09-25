all:
	gcc -Wall -c common.c -o bin/common.o
	gcc -Wall -c minefield.c -o bin/minefield.o
	gcc -Wall client.c bin/common.o bin/minefield.o -o bin/client
	gcc -Wall server.c bin/common.o bin/minefield.o -o bin/server

clean:
	rm -f bin/client bin/server bin/common.o bin/minefield.o
