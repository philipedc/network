#pragma once

#include <stdlib.h>
#include <stdbool.h>

#define SIZE 4

int perform_action(char* path, int type, int coordinates[2], int board[SIZE][SIZE], int revealed[SIZE][SIZE]);
void print_game(int matrix[SIZE][SIZE]);
int check_errors(int type, int coordinates[2], int revealed[SIZE][SIZE]);
int check_win(int revealed[SIZE][SIZE]);
bool check_bomb(int matrix[SIZE][SIZE], int coordinates[2]);