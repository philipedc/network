#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "common.h"

#define MAX_TOPICS 20

struct topic{
    char topic_name[50];
};

struct topic topics[MAX_TOPICS];
unsigned int buf_size = sizeof(struct BlogOperation);

int parse_input(char* input, struct BlogOperation* buf){
    if (strncmp(input, "subscribe in", 12) == 0){
        sscanf(input, "subscribe in %s", buf->topic);

        for (int i = 0; i < MAX_TOPICS; i++){
            if (strcmp(topics[i].topic_name, buf->topic) == 0){
                return -4;
            }
        }

        buf->operation_type = 4;
        return 4;
    }
    if (strcmp(input, "list topics\n") == 0){
        buf->operation_type = 3;
        return 3;
    }
    if (strncmp(input, "publish in", 10) == 0){
        sscanf(input, "publish in %s", buf->topic);
        fgets(buf->content, 2048, stdin);
        buf->operation_type = 2;
        return 2;
    }
    if (strcmp(input, "exit\n") == 0){
        buf->operation_type = 5;
        return -1;
    }
    if (strncmp(input, "unsubscribe", 11) == 0){
        sscanf(input, "unsubscribe %s", buf->topic);
        buf->operation_type = 6;
        return 6;
    }
    return 0;
}

void parse_recv(struct BlogOperation *buf){
    enum {x, NEW_CONNECTION, NEW_POST, LIST_TOPICS, SUBSCRIBE_TOPIC, DISCONNECT, UNSUBSCRIBE_TOPIC};
    switch(buf->operation_type){
        case NEW_POST:
            printf("new post added in %s by %d\n", buf->topic, buf->client_id);
            printf("%s\n", buf->content);
            break;
        case LIST_TOPICS:
            if (strcmp(buf->content, "") == 0){
                printf("no topics available\n");
                break;
            }
            printf("%s\n", buf->content);
            break;
        case SUBSCRIBE_TOPIC:
            for (int i = 0; i < MAX_TOPICS; i++){
                if (strcmp(topics[i].topic_name, "") == 0){
                    strcpy(topics[i].topic_name, buf->topic);
                    break;
                }
            }
    }
}

void *read_thread(void *data){
    struct BlogOperation* buf = malloc(buf_size);
    char* buf_serialized = malloc(buf_size);
    int s = *(int*) data;

    while(1){
        memset(buf, 0, buf_size);
        memset(buf_serialized, 0, buf_size);

        if (recv(s, buf_serialized, buf_size, 0) == -1){
            logexit("recv");
        }
        deserialize_BlogOperation(buf_serialized, buf);

        parse_recv(buf);
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

    struct BlogOperation* buf = malloc(buf_size);
    buf->client_id = 0;
    buf->operation_type = 1;
    buf->server_response = 0;
    strcpy(buf->topic, "");
    strcpy(buf->content, "");

    for (int i = 0; i < MAX_TOPICS; i++){
        strcpy(topics[i].topic_name, "");
    }

    char* buf_serialized = malloc(buf_size);
    serialize_BlogOperation(buf, buf_serialized, buf_size);

    size_t count = send(s, buf_serialized, strlen(buf_serialized)+1, 0);
    if (count != strlen(buf_serialized)+1){
        logexit("send");
    }
    if (recv (s, buf_serialized, buf_size, 0) == -1){ 
        logexit("recv");
    }

    pthread_t tid;
    pthread_create(&tid, NULL, read_thread, &s);

    while(1){
        memset(buf, 0, buf_size);
        memset(buf_serialized, 0, buf_size);

        char input[40];
        fgets(input, 40, stdin);
        fflush(stdin);
        int flag = parse_input(input, buf);

        if (flag == 0) {
            printf("Invalid operation\n");
            continue;
        }

        if (flag == -4){
            printf("error: already subscribed\n");
            continue;
        }

        buf->server_response = 0; // IF THE CLIENT IS SENDING, THE SERVER RESPONSE IS 0
        serialize_BlogOperation(buf, buf_serialized, buf_size);

        int count = send(s, buf_serialized, strlen(buf_serialized)+1, 0);
        if (count != strlen(buf_serialized)+1){
            logexit("send");
        }
        if (flag == -1) break;
    }
    close(s);

    exit(EXIT_SUCCESS);
}