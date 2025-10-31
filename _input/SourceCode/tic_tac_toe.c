// File: tic_tac_toe.c

#include <stdio.h>
#include <stdlib.h>

#define SIZE 3
#define EMPTY ' '
#define PLAYER 'X'
#define AI 'O'

void printBoard(char board[SIZE][SIZE]);
int checkWin(char board[SIZE][SIZE], char player);
int isBoardFull(char board[SIZE][SIZE]);
void playerMove(char board[SIZE][SIZE]);
void aiMove(char board[SIZE][SIZE]);
int minimax(char board[SIZE][SIZE], int depth, int isMaximizing);
int findBestMove(char board[SIZE][SIZE]);

int main() {
    char board[SIZE][SIZE] = {
        {EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY}
    };

    printf("Welcome to Tic-Tac-Toe!\n");
    printBoard(board);

    while (1) {
        playerMove(board);
        printBoard(board);
        if (checkWin(board, PLAYER)) {
            printf("You win!\n");
            break;
        }
        if (isBoardFull(board)) {
            printf("It's a draw!\n");
            break;
        }

        aiMove(board);
        printBoard(board);
        if (checkWin(board, AI)) {
            printf("AI wins!\n");
            break;
        }
        if (isBoardFull(board)) {
            printf("It's a draw!\n");
            break;
        }
    }

    return 0;
}

void printBoard(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf(" %c ", board[i][j]);
            if (j < SIZE - 1) printf("|");
        }
        printf("\n");
        if (i < SIZE - 1) {
            printf("---+---+---\n");
        }
    }
}

int checkWin(char board[SIZE][SIZE], char player) {
    // Check rows and columns
    for (int i = 0; i < SIZE; i++) {
        if ((board[i][0] == player && board[i][1] == player && board[i][2] == player) ||
            (board[0][i] == player && board[1][i] == player && board[2][i] == player)) {
            return 1;
        }
    }
    // Check diagonals
    if ((board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
        (board[0][2] == player && board[1][1] == player && board[2][0] == player)) {
        return 1;
    }
    return 0;
}

int isBoardFull(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (board[i][j] == EMPTY)
                return 0;
    return 1;
}

void playerMove(char board[SIZE][SIZE]) {
    int row, col;
    while (1) {
        printf("Enter your move (row and column): ");
        scanf("%d %d", &row, &col);
        if (row >= 0 && row < SIZE && col >= 0 && col < SIZE && board[row][col] == EMPTY) {
            board[row][col] = PLAYER;
            break;
        } else {
            printf("Invalid move. Try again.\n");
        }
    }
}

void aiMove(char board[SIZE][SIZE]) {
    int bestMove = findBestMove(board);
    board[bestMove / SIZE][bestMove % SIZE] = AI;
}

int minimax(char board[SIZE][SIZE], int depth, int isMaximizing) {
    if (checkWin(board, AI)) return 10 - depth;
    if (checkWin(board, PLAYER)) return depth - 10;
    if (isBoardFull(board)) return 0;

    if (isMaximizing) {
        int bestScore = -1000;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (board[i][j] == EMPTY) {
                    board[i][j] = AI;
                    int score = minimax(board, depth + 1, 0);
                    board[i][j] = EMPTY;
                    if (score > bestScore) bestScore = score;
                }
            }
        }
        return bestScore;
    } else {
        int bestScore = 1000;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (board[i][j] == EMPTY) {
                    board[i][j] = PLAYER;
                    int score = minimax(board, depth + 1, 1);
                    board[i][j] = EMPTY;
                    if (score < bestScore) bestScore = score;
                }
            }
        }
        return bestScore;
    }
}

int findBestMove(char board[SIZE][SIZE]) {
    int bestValue = -1000;
    int move = -1;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == EMPTY) {
                board[i][j] = AI;
                int moveValue = minimax(board, 0, 0);
                board[i][j] = EMPTY;
                if (moveValue > bestValue) {
                    bestValue = moveValue;
                    move = i * SIZE + j;
                }
            }
        }
    }
    return move;
}
