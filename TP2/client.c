#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "common.h"

#define BUFFSIZE 1024

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

    char addrstr[BUFFSIZE];
    addrtostr(addr, addrstr, BUFFSIZE);

    printf("Connected to %s\n", addrstr);

    char buf[BUFFSIZE];
    memset(buf, 0, BUFFSIZE);
    printf("Data sent: ");
    fgets(buf, BUFFSIZE-1, stdin);
    size_t count = send(s, buf, strlen(buf)+1, 0);
    if (count != strlen(buf)+1) logexit("send");

    memset(buf, 0, BUFFSIZE);
    unsigned total = 0;
    while(1){
        count = recv(s, buf + total, BUFFSIZE - total, 0);
        if (count == 0) break; // connection terminated
        total += count;
    }
    close(s);

    printf("Received %u bytes\n", total);
    printf("Data received: %s\n", buf);
    exit(EXIT_SUCCESS);
}