#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>
#include "common.h"


void serialize_BlogOperation(struct BlogOperation *input, char *output, unsigned int buf_size){
    snprintf(output, buf_size, "%d:%d:%d:%s:%s", input->client_id, input->operation_type, input->server_response, input->topic, input->content);
}

void deserialize_BlogOperation(char *input, struct BlogOperation *output){
    sscanf(input, "%d:%d:%d:%19[^:]:%19[^\n]", &output->client_id, &output->operation_type, &output->server_response, output->topic, output->content);
}

void logexit(const char *str){
    perror(str);
    exit(EXIT_FAILURE);
}

int addrparser(const char *addrstr, const char *portstr, struct sockaddr_storage* storage){
    if (addrstr == NULL || portstr == NULL) return -1;

    uint16_t port = (uint16_t)atoi(portstr);
    if (port == 0) return -1;

    port = htons(port);

    struct in_addr inaddr4; // IPv4
    if (inet_pton(AF_INET, addrstr, &inaddr4)){
        struct sockaddr_in *addr4 = (struct sockaddr_in *) storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return 0;
    }

    struct in6_addr inaddr6; // IPv6
    if (inet_pton(AF_INET6, addrstr, &inaddr6)){
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *) storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));
        return 0;
    }

    return -1;
}

void addrtostr(const struct sockaddr *addr, char* str, size_t strsize){
    
    int version;
    char addrstr[INET6_ADDRSTRLEN + 1] = "";
    uint16_t port;


    if (addr->sa_family == AF_INET){
        version = 4;
        struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
        if (inet_ntop(AF_INET, &(addr4->sin_addr), addrstr, INET6_ADDRSTRLEN + 1) == NULL) logexit("inet_ntop");
        port = ntohs(addr4->sin_port); // Network to host short
    } 
    else if (addr->sa_family == AF_INET6){
        version = 6;
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
        if (inet_ntop(AF_INET6, &(addr6->sin6_addr), addrstr, INET6_ADDRSTRLEN + 1) == NULL) logexit("inet_ntop");
        port = ntohs(addr6->sin6_port); // Network to host short
    }
    else return;
    if (str) snprintf(str, strsize, "IPv%d %s %hu", version, addrstr, port);
}

int server_sockaddr_init(const char* proto, const char* portstr, struct sockaddr_storage * storage){

    uint16_t port = (uint16_t)atoi(portstr);
    if (port == 0) return -1;
    port = htons(port);

    memset(storage, 0, sizeof(*storage));
    if (strcmp(proto, "v4") == 0){
        struct sockaddr_in *addr4 = (struct sockaddr_in *) storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr.s_addr = INADDR_ANY;
        return 0;
    }
    else if (strcmp(proto, "v6") == 0){
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *) storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        addr6->sin6_addr = in6addr_any;
        return 0;
    }
    else return -1;
}