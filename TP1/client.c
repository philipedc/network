#include "common.h"

#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>


struct action{
    int type;
    int coordinates[2];
    int board[4][4];
};

void usage(int argc, char* argv[]){
    printf("Usage: %s <IP> <Port Number>\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]){
    const int BUFFSIZE = sizeof(struct action);

    if (argc != 3) usage(argc, argv);

    struct sockaddr_storage storage;
    if (addrparser(argv[1], argv[2], &storage) != 0) usage(argc, argv);

    int s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1) logexit("socket");

    struct sockaddr *addr = (struct sockaddr *) &storage;
    if (connect(s, addr, sizeof(storage)) != 0) logexit("connect");

    char addrstr[BUFFSIZE];
    addrtostr(addr, addrstr, BUFFSIZE);

    printf("Connected to %s\n", addrstr);

    struct action buf[BUFFSIZE];
    while(1){
        memset(buf, 0, BUFFSIZE);
        scanf("%d", &buf->type);
        if (buf->type == 7) break;
        printf("Data sent: ");
        
        size_t count = send(s, buf, BUFFSIZE+1, 0);
        if (count != BUFFSIZE+1) logexit("send");

        memset(buf, 0, BUFFSIZE);
        if (recv (s, buf, BUFFSIZE+1, 0) == -1) logexit("recv");
        printf("Data received: %d\n", buf->type);
    }
    close(s);
    exit(EXIT_SUCCESS);
}