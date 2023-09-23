all:
	gcc -Wall -c common.c
	gcc -Wall -c minefield.c
	gcc -Wall client.c common.o minefield.o -o client
	gcc -Wall server.c common.o minefield.o -o server

clean:
	rm -f client server common.o
