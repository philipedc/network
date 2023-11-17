#include "common.h"

#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h> 

#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define PENDING_CONNECTIONS 10
#define MAX_CLIENTS 20
#define MAX_TOPICS 20

struct client_data{
    int csock;
    struct sockaddr_storage storage;
    int client_id;
    struct topic *topics[MAX_TOPICS];
};

int client_id = 0;

struct topic{
    char topic_name[50];
};

struct topic topics[MAX_TOPICS];
struct client_data *clients[MAX_CLIENTS]; 
unsigned int buf_size = sizeof(struct BlogOperation);


int Operation(struct BlogOperation *buf, struct client_data *cdata){
    buf->server_response = 1; // IF THE SERVER IS SENDING, THE SERVER RESPONSE IS 1
    enum {x, NEW_CONNECTION, NEW_POST, LIST_TOPICS, SUBSCRIBE_TOPIC, DISCONNECT, UNSUBSCRIBE_TOPIC};
    switch(buf->operation_type){
        case NEW_CONNECTION:
            buf->server_response = 1;
            break;
        case NEW_POST:
            return 2;
            break;
        case LIST_TOPICS:
            for (int i = 0; i < MAX_TOPICS; i++){
                if (strcmp(topics[i].topic_name, "") != 0){
                    strcat(buf->content, topics[i].topic_name);
                    strcat(buf->content, ",");
                }
            }
            size_t contentLen = strlen(buf->content);
            if (contentLen > 0) {
                buf->content[contentLen - 1] = '\0';
            }
            break;
        case SUBSCRIBE_TOPIC:;
            int first_empty = -1;
            for (int i = 0; i < MAX_TOPICS; i++){
                if (strcmp(cdata->topics[i]->topic_name, "") == 0 && first_empty == -1){
                    first_empty = i;
                    break;
                }
            }
            if (first_empty != -1){
                strcpy(cdata->topics[first_empty]->topic_name, buf->topic);
                printf("Client %d subscribed to %s\n", cdata->client_id, buf->topic);
            }
            for (int i = 0; i < MAX_TOPICS; i++){
                if (strcmp(topics[i].topic_name, "") == 0){
                    strcpy(topics[i].topic_name, buf->topic);
                    break;
                }
            }
            break;
        case DISCONNECT:
            printf("client %d disconnected\n", cdata->client_id);
            cdata->client_id = -1;
            for (int i = 0; i < MAX_TOPICS; i++){
                strcpy(cdata->topics[i]->topic_name, "");
            }
            return -1;
        case UNSUBSCRIBE_TOPIC:
            for (int i = 0; i < MAX_TOPICS; i++){
                if (strcmp(cdata->topics[i]->topic_name, buf->topic) == 0){
                    strcpy(cdata->topics[i]->topic_name, "");
                    printf("Client %d unsubscribed from %s\n", cdata->client_id, buf->topic);
                }
            }
        default:
            printf("Invalid operation\n");
            break;
    }
    return 0;
}

void *client_thread(void *data){
    
    struct client_data *cdata = (struct client_data*) data;
    struct sockaddr *caddr = (struct sockaddr *) &cdata->storage;

    char caddrstr[buf_size];
    addrtostr(caddr, caddrstr, buf_size);
    printf("client %d connected\n", cdata->client_id);

    struct BlogOperation *buf = malloc(buf_size);
    char* buf_serialized = (char*) malloc(buf_size);

    while(1){
        memset(buf, 0, buf_size);
        memset(buf_serialized, 0, buf_size);
        int count = recv(cdata->csock, buf_serialized, buf_size, 0);
        printf("[MSG]: %s", buf_serialized);
        deserialize_BlogOperation(buf_serialized, buf);

        int flag = Operation(buf, cdata);

        if (flag == -1){
            break;
        }
        else if (flag == 2){
            for (int i = 0; i < MAX_CLIENTS; i++){
                if (clients[i]->client_id != -1){
                    for (int j = 0; j < MAX_TOPICS; j++){
                        if (strcmp(clients[i]->topics[j]->topic_name, buf->topic) == 0){
                            buf->client_id = cdata->client_id;
                            serialize_BlogOperation(buf, buf_serialized, buf_size);
                            count = send(clients[i]->csock, buf_serialized, strlen(buf_serialized)+1, 0);
                            if (count != strlen(buf_serialized)+1) logexit("send");
                        }
                    }
                }
            }
        }
        else{
            buf->client_id = cdata->client_id;
            serialize_BlogOperation(buf, buf_serialized, buf_size);
            count = send(cdata->csock, buf_serialized, strlen(buf_serialized)+1, 0);
            if (count != strlen(buf_serialized)+1) logexit("send");
        }
    }
    close(cdata->csock);
    free(buf);
    free(buf_serialized);
    pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]){
    // Initializing topics array
    for (int i = 0; i < MAX_TOPICS; i++){
        strcpy(topics[i].topic_name, "");
    }

    struct sockaddr_storage storage;
    if (server_sockaddr_init(argv[1], argv[2], &storage) != 0){
        printf("Usage: <v4/v6> <Port Number>\n");
    }

    int s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1) logexit("socket");

    int enable = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0) logexit("setsockopt"); 

    struct sockaddr *addr = (struct sockaddr *) &storage;
    if (bind(s, addr, sizeof(storage)) != 0) logexit("bind");
    if (listen(s, PENDING_CONNECTIONS) != 0) logexit("listen");

    char addrstr[buf_size];
    addrtostr(addr, addrstr, buf_size);

    // Initializing clients array(to keep track of clients inside the thread)
    for (int i = 0; i < MAX_CLIENTS; i++){
        clients[i] = (struct client_data *)malloc(sizeof(struct client_data));
        clients[i]->client_id = -1;
        for (int j = 0; j < MAX_TOPICS; j++){
            clients[i]->topics[j] = (struct topic *)malloc(sizeof(struct topic)*MAX_TOPICS);
            strcpy(clients[i]->topics[j]->topic_name, "");
        }
    }
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

        for (int i = 0; i < MAX_CLIENTS; i++){
            if (clients[i]->client_id == -1){
                clients[i] = cdata;
                cdata->client_id = i + 1;
                break;
            }
        }
        for (int i = 0; i < MAX_TOPICS; i++){
            cdata->topics[i] = (struct topic*) malloc(sizeof(struct topic));
            strcpy(cdata->topics[i]->topic_name, "");
        }

        client_id++;
        pthread_t tid;
        pthread_create(&tid, NULL, client_thread, cdata);
    }
    
    exit(EXIT_SUCCESS);
}

