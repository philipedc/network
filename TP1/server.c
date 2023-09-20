#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h> 

#include "common.h"

#define PENDING_CONNECTIONS 10

struct action{
    int type;
    int coordinates[2];
    int board[4][4];
};

int main(int argc, char* argv[]){
    const int BUFFSIZE = sizeof(struct action);

    // if (argc != 5){
    //     printf("Usage: <v4/v6> <Port Number> -i <Path>\n");
    //     exit(EXIT_FAILURE);
    // }
    char *input_path;
    int opt;
    // while((opt = getopt(argc, argv, "i:")) != -1) 
    // { 
    //     switch(opt) 
    //     { 
    //         case 'i':
    //             input_path = optarg;
    //             break;

    //         default:
    //             printf("Usage: <v4/v6> <Port Number> -i <Path>\n");
    //             exit(EXIT_FAILURE);
    //     }
    // }
    // printf("Input path: %s\n", input_path);

    struct sockaddr_storage storage;
    if (server_sockaddr_init(argv[1], argv[2], &storage) != 0){
        printf("Usage: <v4/v6> <Port Number> -i <Path>\n");
    }

    int s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1) logexit("socket");

    int enable = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0) logexit("setsockopt"); 

    struct sockaddr *addr = (struct sockaddr *) &storage;
    if (bind(s, addr, sizeof(storage)) != 0) logexit("bind");
    if (listen(s, PENDING_CONNECTIONS) != 0) logexit("listen");

    char addrstr[BUFFSIZE];
    addrtostr(addr, addrstr, BUFFSIZE);
    printf("Bound to %s, waiting connections\n", addrstr);

    while(1){
        struct sockaddr_storage *cstorage;
        struct sockaddr *caddr = (struct sockaddr *) &cstorage;
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1) logexit("accept");

        char caddrstr[BUFFSIZE];
        addrtostr(caddr, caddrstr, BUFFSIZE);
        printf("[log] connection from %s\n", caddrstr);

        struct action buf[BUFFSIZE];
        while(1){
            memset(buf, 0, BUFFSIZE);
            size_t count = recv(csock, buf, BUFFSIZE+1, 0);
            printf("[MSG] %s, %d bytes: %d\n", caddrstr, (int)count, buf->type);
            // sprintf(buf, "Remote endpoint: %.1000s\n", caddrstr);

            count = send(csock, buf, BUFFSIZE+1, 0);
            if (count != BUFFSIZE+1) logexit("send");
        }
        close(csock);
    }
}