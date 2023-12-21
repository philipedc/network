all:
	gcc -Wall -c common.c -o bin/common.o
	gcc -Wall -pthread client.c bin/common.o -o bin/client
	gcc -Wall -pthread server.c bin/common.o -o bin/server

clean:
	rm -f bin/client bin/server bin/common.o