#include "common.h"

#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h> 

#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>



#define BUFFSIZE 1024
#define PENDING_CONNECTIONS 10

struct client_data{
    int csock;
    struct sockaddr_storage storage;
};

void *client_thread(void *data){
    
    struct client_data*cdata = (struct client_data*) data;
    struct sockaddr *caddr = (struct sockaddr *) &cdata->storage;

    char caddrstr[BUFFSIZE];
    addrtostr(caddr, caddrstr, BUFFSIZE);
    printf("[log] connection from %s\n", caddrstr);

    char buf[BUFFSIZE];
    memset(buf, 0, BUFFSIZE);
    size_t count = recv(cdata->csock, buf, BUFFSIZE, 0);
    printf("[MSG] %s, %d bytes: %s\n", caddrstr, (int)count, buf);
    sprintf(buf, "Remote endpoint: %.1000s\n", caddrstr);

    count = send(cdata->csock, buf, strlen(buf)+1, 0);
    if (count != strlen(buf)+1) logexit("send");

    close(cdata->csock);

    pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]){

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
        
        struct client_data *cdata = malloc(sizeof(*cdata));
        if (cdata == NULL) logexit("malloc");
        cdata->csock = csock;
        memcpy(&cdata->storage, &cstorage, sizeof(cstorage));

        pthread_t tid;
        pthread_create(&tid, NULL, client_thread, cdata);
    }
    
    exit(EXIT_SUCCESS);
}

