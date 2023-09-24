#include "common.h"
#include "minefield.h"

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
    memset(buf, 0, BUFFSIZE+1);
    while(1){
        char input[20];
        char* command;
        char* str_coord;
        fgets(input, 20, stdin);
        command = strtok(input, " ");
        command[strcspn(command, "\n")] = 0; //TRIMMING
        str_coord = strtok(NULL, " ");
        if (str_coord != NULL){
            buf->coordinates[0] = atoi(strtok(str_coord, ","));
            buf->coordinates[1] = atoi(strtok(NULL, ","));
        }

        // printf("<%s %d %d>\n", command, buf->coordinates[0], buf->coordinates[1]);

        if (strcmp(command, "start") == 0) buf->type = 0;
        else if (strcmp(command, "reveal") == 0) buf->type = 1;
        else if (strcmp(command, "flag") == 0) buf->type = 2;
        else if(strcmp(command, "remove_flag") == 0) buf->type = 4;
        else if(strcmp(command, "reset") == 0) buf->type = 5;
        else if(strcmp(command, "game_over") == 0) buf->type = 8;
        else if(strcmp(command, "exit") == 0) break;
        else buf->type = -1;
        // printf("Data sent: {type: %d, coordinates: [%d, %d]}", buf->type, buf->coordinates[0], buf->coordinates[1]);
        if (check_errors(buf->type, buf->coordinates, buf->board) != 0) continue;

        size_t count = send(s, buf, BUFFSIZE+1, 0);
        if (count != BUFFSIZE+1) logexit("send");

        memset(buf, 0, BUFFSIZE);
        if (recv (s, buf, BUFFSIZE+1, 0) == -1) logexit("recv");

        if (buf->type == 6){
            printf("YOU WIN!\n");
        }

        if (check_bomb(buf->board, buf->coordinates) == true){
            printf("GAME OVER!\n");
        }
        print_game(buf->board);
    }
    close(s);
    exit(EXIT_SUCCESS);
}