#pragma once

#include <stdlib.h>
#include <stdbool.h>

#define SIZE 4

struct action{
    int type;
    int coordinates[2];
    int board[4][4];
};

int play_game(char* path, struct action *buf, int board[SIZE][SIZE], int revealed[SIZE][SIZE]);
void print_game(int matrix[SIZE][SIZE]);
void initialize_game(const char* path, int board[SIZE][SIZE], int revealed[SIZE][SIZE]);