#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h> 

#include "minefield.h"
#include "common.h"

#define PENDING_CONNECTIONS 10
#define SIZE 4

struct action{
    int type;
    int coordinates[2];
    int board[4][4];
};

int update_matrix(int matrix[SIZE][SIZE], struct action *buf){
    for (int i = 0; i < SIZE; i++){
        for (int y = 0; y < SIZE; y++){
            buf->board[i][y] = matrix[i][y];
        }
    }
    return 0;
}

int play_game(char* path, int board[SIZE][SIZE], int revealed[SIZE][SIZE], struct action *buf){

    perform_action(path, buf->type, buf->coordinates, board, revealed);
    if (check_win(revealed) == 1){
        printf("You won!\n");
        buf->type = 6;
    }
    update_matrix(revealed, buf);
    return 0;
}

int main(int argc, char* argv[]){
    const int BUFFSIZE = sizeof(struct action);

    if (argc != 5){
        printf("Usage: <v4/v6> <Port Number> -i <Path>\n");
        exit(EXIT_FAILURE);
    }

    char* address_family = argv[1];
    char* port = argv[2];

    char *path = "input.txt";
    int opt;
    while((opt = getopt(argc, argv, "i:")) != -1) 
    { 
        switch(opt) 
        { 
            case 'i':
                path = optarg;
                break;

            default:
                printf("Usage: <v4/v6> <Port Number> -i <Path>\n");
                exit(EXIT_FAILURE);
        }
    }

    struct sockaddr_storage storage;
    if (server_sockaddr_init(address_family, port, &storage) != 0){
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

    while(1){ //Listening
        struct sockaddr_storage *cstorage;
        struct sockaddr *caddr = (struct sockaddr *) &cstorage;
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1) logexit("accept");

        char caddrstr[BUFFSIZE];
        addrtostr(caddr, caddrstr, BUFFSIZE);
        printf("[log] connection from %s\n", caddrstr);

        struct action buf[BUFFSIZE];
        while(1){ // Reading input from client
            memset(buf, 0, BUFFSIZE);
            size_t count = recv(csock, buf, BUFFSIZE+1, 0);

            if (buf->type == 7){
                printf("[log] connection closed from %s\n", caddrstr);
                break;
            }

            int board[SIZE][SIZE];
            int revealed[SIZE][SIZE];
            play_game(path, board, revealed, buf);

            printf("[MSG] %s, %d bytes {command: %d, coordinates: %d %d}\n", caddrstr, (int)count, buf->type, buf->coordinates[0], buf->coordinates[1]);

            count = send(csock, buf, BUFFSIZE+1, 0);
            if (count != BUFFSIZE+1) logexit("send");
        }
        close(csock);
    }
}