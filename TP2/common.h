#pragma once

#include <stdlib.h>

#include <arpa/inet.h>


struct BlogOperation {
    int client_id;
    int operation_type;
    int server_response;
    char topic[50];
    char content[2048];
};


void serialize_BlogOperation(struct BlogOperation *input, char *output, unsigned int buf_size);
void deserialize_BlogOperation(char *input, struct BlogOperation *output);

void logexit(const char *str);

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize);
int addrparser(const char *addrstr, const char *portstr, struct sockaddr_storage* storage);
int server_sockaddr_init(const char* proto, const char* portstr, struct sockaddr_storage * storage);

