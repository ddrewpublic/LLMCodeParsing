// File: sudoku_solver_generator.c

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE 9
#define SUBGRID_SIZE 3

// Function prototypes
void printBoard(int board[SIZE][SIZE]);
bool isSafe(int board[SIZE][SIZE], int row, int col, int num);
bool solveSudoku(int board[SIZE][SIZE]);
bool findEmptyLocation(int board[SIZE][SIZE], int *row, int *col);
void generateSudoku(int board[SIZE][SIZE], int numClues);
void fillBoard(int board[SIZE][SIZE]);
void removeNumbers(int board[SIZE][SIZE], int numClues);

// Print the Sudoku board
void printBoard(int board[SIZE][SIZE]) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            printf("%d ", board[row][col]);
        }
        printf("\n");
    }
}

// Check if it's safe to place a number in the given position
bool isSafe(int board[SIZE][SIZE], int row, int col, int num) {
    for (int x = 0; x < SIZE; x++) {
        if (board[row][x] == num || board[x][col] == num ||
            board[row - row % SUBGRID_SIZE + x / SUBGRID_SIZE][col - col % SUBGRID_SIZE + x % SUBGRID_SIZE] == num) {
            return false;
        }
    }
    return true;
}

// Solve the Sudoku using backtracking
bool solveSudoku(int board[SIZE][SIZE]) {
    int row, col;
    if (!findEmptyLocation(board, &row, &col)) {
        return true; // Solved
    }
    
    for (int num = 1; num <= SIZE; num++) {
        if (isSafe(board, row, col, num)) {
            board[row][col] = num;
            if (solveSudoku(board)) {
                return true;
            }
            board[row][col] = 0; // Backtrack
        }
    }
    return false; // Trigger backtracking
}

// Find an empty location on the board (represented by 0)
bool findEmptyLocation(int board[SIZE][SIZE], int *row, int *col) {
    for (*row = 0; *row < SIZE; (*row)++) {
        for (*col = 0; *col < SIZE; (*col)++) {
            if (board[*row][*col] == 0) {
                return true;
            }
        }
    }
    return false; // No empty location
}

// Generate a complete Sudoku board
void generateSudoku(int board[SIZE][SIZE], int numClues) {
    fillBoard(board);
    removeNumbers(board, numClues);
}

// Fill the board with a valid Sudoku solution
void fillBoard(int board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = 0; // Initialize board
        }
    }
    solveSudoku(board); // Fill the board
}

// Remove numbers from the board to create a puzzle
void removeNumbers(int board[SIZE][SIZE], int numClues) {
    int count = SIZE * SIZE - numClues;
    while (count > 0) {
        int i = rand() % SIZE;
        int j = rand() % SIZE;
        if (board[i][j] != 0) {
            board[i][j] = 0; // Remove the number
            count--;
        }
    }
}

int main() {
    int board[SIZE][SIZE] = {0};
    int numClues = 40; // Change the number of clues here

    // Seed random number generator
    srand(time(NULL));

    // Generate Sudoku puzzle
    generateSudoku(board, numClues);
    printf("Generated Sudoku Puzzle:\n");
    printBoard(board);

    // Solve the Sudoku puzzle
    if (solveSudoku(board)) {
        printf("\nSolved Sudoku Puzzle:\n");
        printBoard(board);
    } else {
        printf("No solution exists.\n");
    }

    return 0;
}
