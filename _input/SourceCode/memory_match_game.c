// File: memory_match_game.c

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define SIZE 4 // Size of the grid (4x4)
#define PAIRS (SIZE * SIZE) / 2 // Number of pairs

// Function prototypes
void initialize_board(char board[SIZE][SIZE], char symbols[PAIRS]);
void shuffle_symbols(char symbols[PAIRS]);
void print_board(char board[SIZE][SIZE], bool revealed[SIZE][SIZE]);
bool is_pair(char board[SIZE][SIZE], int x1, int y1, int x2, int y2);
bool is_winner(bool revealed[SIZE][SIZE]);

int main() {
    char board[SIZE][SIZE];
    char symbols[PAIRS] = {'A', 'A', 'B', 'B', 'C', 'C', 'D', 'D', 
                           'E', 'E', 'F', 'F', 'G', 'G', 'H', 'H'};
    bool revealed[SIZE][SIZE] = {false}; // Tracks revealed positions
    int x1, y1, x2, y2;
    int turns = 0;

    // Initialize and shuffle the board
    shuffle_symbols(symbols);
    initialize_board(board, symbols);

    // Game loop
    while (!is_winner(revealed)) {
        print_board(board, revealed);
        printf("Enter coordinates of first card (row and column): ");
        scanf("%d %d", &x1, &y1);
        printf("Enter coordinates of second card (row and column): ");
        scanf("%d %d", &x2, &y2);

        // Check if the coordinates are valid
        if (x1 < 0 || x1 >= SIZE || y1 < 0 || y1 >= SIZE || 
            x2 < 0 || x2 >= SIZE || y2 < 0 || y2 >= SIZE || 
            (x1 == x2 && y1 == y2) || revealed[x1][y1] || revealed[x2][y2]) {
            printf("Invalid input. Try again.\n");
            continue;
        }

        // Reveal the chosen cards
        revealed[x1][y1] = true;
        revealed[x2][y2] = true;

        // Check for a match
        if (!is_pair(board, x1, y1, x2, y2)) {
            printf("Not a match! Try again.\n");
            revealed[x1][y1] = false;
            revealed[x2][y2] = false;
        } else {
            printf("You found a match!\n");
        }

        turns++;
    }

    print_board(board, revealed);
    printf("Congratulations! You found all pairs in %d turns.\n", turns);
    return 0;
}

// Initialize the board with symbols
void initialize_board(char board[SIZE][SIZE], char symbols[PAIRS]) {
    int k = 0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = symbols[k++];
        }
    }
}

// Shuffle the symbols array
void shuffle_symbols(char symbols[PAIRS]) {
    srand(time(NULL)); // Seed for random number generation
    for (int i = PAIRS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        char temp = symbols[i];
        symbols[i] = symbols[j];
        symbols[j] = temp;
    }
}

// Print the current state of the board
void print_board(char board[SIZE][SIZE], bool revealed[SIZE][SIZE]) {
    printf("Current board:\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (revealed[i][j]) {
                printf("%c ", board[i][j]);
            } else {
                printf("* ");
            }
        }
        printf("\n");
    }
}

// Check if two selected cards are a pair
bool is_pair(char board[SIZE][SIZE], int x1, int y1, int x2, int y2) {
    return board[x1][y1] == board[x2][y2];
}

// Check if the player has won
bool is_winner(bool revealed[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (!revealed[i][j]) {
                return false;
            }
        }
    }
    return true;
}
