#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define SIZE 4
#define BOMB -1
#define OBFUSCATED -2
#define FLAGGED -3

struct action{
    int type;
    int coordinates[2];
    int board[4][4];
};

int update_matrix(int matrix[SIZE][SIZE], struct action *buf){
    for (int i = 0; i < SIZE; i++){
        for (int y = 0; y < SIZE; y++){
            buf->board[i][y] = matrix[i][y];
        }
    }
    return 0;
}

void reveal_board(int board[SIZE][SIZE], int revealed[SIZE][SIZE]){
    for (int i = 0; i < SIZE; i++){
        for (int y = 0; y < SIZE; y++){
            revealed[i][y] = board[i][y];
        }
    }
}

bool check_bomb(int matrix[SIZE][SIZE], int coordinates[2]){
    if (matrix[coordinates[0]][coordinates[1]] == BOMB) return true;
    return false;
}

bool isRevealed(int matrix[SIZE][SIZE], int coordinates[2]){
    if (matrix[coordinates[0]][coordinates[1]] == OBFUSCATED || matrix[coordinates[0]][coordinates[1]] == FLAGGED) return false;
    return true;
}

void reset_matrix(int matrix[SIZE][SIZE]){
    for (int i = 0; i < SIZE; i++){
        for (int y = 0; y < SIZE; y++){
            matrix[i][y] = OBFUSCATED;
        }
    }
}

void initialize_game(const char* path, int board[SIZE][SIZE], int revealed[SIZE][SIZE]){

    reset_matrix(revealed);

    FILE *fptr;
    fptr = fopen(path, "r");
    char line[20];

    int height = 0;
    while(fgets(line, sizeof(line), fptr) != NULL){
        char *token = strtok(line, ",");
        int i = 0;
        while(token != NULL){
            board[height][i] = atoi(token);
            token = strtok(NULL, ",");
            i++;
        }
        height++;
    }

    fclose(fptr);
}
void print_game(int matrix[SIZE][SIZE]){

    for (int i = 0; i < SIZE; i++){
        for (int j = 0; j < SIZE; j++){
            if (matrix[i][j] == BOMB) printf("*\t\t");
            else if (matrix[i][j] == OBFUSCATED) printf("-\t\t");
            else if (matrix[i][j] == FLAGGED) printf("<\t\t");
            else printf("%d\t\t", matrix[i][j]);
        }
        printf("\n");
    }
}

int check_errors(int type, int coordinates[2], int revealed[SIZE][SIZE]){
    if (coordinates[0] > SIZE-1 || coordinates[0] < 0 || coordinates[1] > SIZE-1 || coordinates[1] < 0){
        printf("error: invalid cell\n");
        return -1;
    }
    else if (type == 1){ // Reveal
        if (isRevealed(revealed, coordinates)){
            printf("error: already revealed\n");
            return -2;
        }
    }
    else if (type == 2){
        if (revealed[coordinates[0]][coordinates[1]] == FLAGGED){
            printf("error: cell already has a flag\n");
            return -3;
        }
        else if (isRevealed(revealed, coordinates)){
            printf("error: cell already revealed\n");
            return -4;
        }
    return 0;
    }
    else if (type == 4){ // Remove FLAGGED
        if (revealed[coordinates[0]][coordinates[1]] != FLAGGED){
            printf("error: cell does not have a flag\n");
            return -5;
        }
    }
    else if (type == -1 || type == 3 || type == 6 || type > 8){ 
        printf("error: command not found\n");
        return -6;
    }
    return 0;
}

int play_game(char* path, struct action *buf, int board[SIZE][SIZE], int revealed[SIZE][SIZE]){

    int coordinates[2];
    coordinates[0] = buf->coordinates[0];
    coordinates[1] = buf->coordinates[1]; 

    if (buf->type == 0){
        initialize_game(path, board, revealed);
    }

    if (buf->type == 1){ 
        revealed[coordinates[0]][coordinates[1]] = board[coordinates[0]][coordinates[1]];
        if (check_win(revealed) == 1){
            buf->type = 6;
            reveal_board(board, revealed);
        }
        if (check_bomb(board, buf->coordinates) == true){
            buf->type = 8;
            reveal_board(board, revealed);
        }
    }
    
    else if (buf->type == 2){
        revealed[coordinates[0]][coordinates[1]] = FLAGGED;
    }
    else if (buf->type == 4){
        revealed[coordinates[0]][coordinates[1]] = OBFUSCATED;
    }
    else if (buf->type == 5){
        printf("starting new game\n");
        reset_matrix(revealed);
    }
    update_matrix(revealed, buf);
    return 0;
}

//Return 0 if game is not over, 1 if game is over
int check_win(int revealed[SIZE][SIZE]){
    int count = 0;
    int coordinates[2];
    for (int i = 0; i < SIZE; i++){
        for (int y = 0; y < SIZE; y++){
            coordinates[0] = i;
            coordinates[1] = y;
            if (isRevealed(revealed, coordinates)) count++;
        }
    }
    printf("count: %d\n", count);
    if (count == SIZE*SIZE-3) return 1; // 3 is the number of bombs
    return 0;
}