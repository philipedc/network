#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "common.h"

unsigned int buf_size = sizeof(struct BlogOperation);

void parse_input(char* input, struct BlogOperation* buf){
    if (strncmp(input, "subscribe in", 12) == 0){
        sscanf(input, "subscribe in %s", buf->topic);
        buf->operation_type = 4;
    }
    if (strcmp(input, "list topics\n") == 0){
        buf->operation_type = 3;    
    }
    if (strncmp(input, "publish in", 10) == 0){
        sscanf(input, "publish in %s", buf->topic);
        fgets(buf->content, 2048, stdin);
        buf->operation_type = 2;
    }
    if (strcmp(input, "exit\n") == 0){
        buf->operation_type = 5;
    }
    if (strncmp(input, "unsubscribe", 11) == 0){
        sscanf(input, "unsubscribe %s", buf->topic);
        buf->operation_type = 6;
    }

}


void usage(int argc, char* argv[]){
    printf("Usage: %s <IP> <Port Number>\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]){
    if (argc != 3) usage(argc, argv);

    struct sockaddr_storage storage;
    if (addrparser(argv[1], argv[2], &storage) != 0) usage(argc, argv);

    int s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1) logexit("socket");

    struct sockaddr *addr = (struct sockaddr *) &storage;
    if (connect(s, addr, sizeof(storage)) != 0) logexit("connect");

    char addrstr[buf_size];
    addrtostr(addr, addrstr, buf_size);

    printf("Connected to %s\n", addrstr);

    struct BlogOperation* buf = malloc(buf_size);
    buf->client_id = 0;
    buf->operation_type = 1;
    buf->server_response = 0;
    strcpy(buf->topic, "");
    strcpy(buf->content, "");


    char* buf_serialized = malloc(buf_size);
    serialize_BlogOperation(buf, buf_serialized, buf_size);

    size_t count = send(s, buf_serialized, strlen(buf_serialized)+1, 0);
    if (count != strlen(buf_serialized)+1){
        logexit("send");
    }
    printf("sent: %s\n", buf_serialized);
    if (recv (s, buf_serialized, buf_size, 0) == -1){ 
        logexit("recv");
    }
    printf("data received: %s\n", buf_serialized);

    while(1){
        char input[40];
        fgets(input, 40, stdin);
        fflush(stdin);
        
        parse_input(input, buf);

        serialize_BlogOperation(buf, buf_serialized, buf_size);

        int count = send(s, buf_serialized, strlen(buf_serialized)+1, 0);
        if (count != strlen(buf_serialized)+1){
            printf("count: %d strlen: %ld\n", count, strlen(buf_serialized)+1);
            logexit("send");
        }
        memset(buf_serialized, 0, buf_size);
        count = recv(s, buf_serialized, buf_size, 0);

        deserialize_BlogOperation(buf_serialized, buf);
        printf("Data received: %s\n", buf_serialized);
            
    }
    close(s);

    exit(EXIT_SUCCESS);
}