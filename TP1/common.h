#pragma once

#include <stdlib.h>

#include <arpa/inet.h>

void logexit(const char *str);

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize);

int addrparser(const char *addrstr, const char *portstr, struct sockaddr_storage* storage);

int server_sockaddr_init(const char* proto, const char* portstr, struct sockaddr_storage * storage);

void read_board(int board[4][4]);