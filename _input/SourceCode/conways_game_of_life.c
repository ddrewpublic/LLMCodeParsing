// File: conways_game_of_life.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define WIDTH 20
#define HEIGHT 20

// Function prototypes
void initialize_grid(int grid[HEIGHT][WIDTH]);
void print_grid(int grid[HEIGHT][WIDTH]);
void update_grid(int grid[HEIGHT][WIDTH], int new_grid[HEIGHT][WIDTH]);
int count_neighbors(int grid[HEIGHT][WIDTH], int x, int y);

int main() {
    int grid[HEIGHT][WIDTH];
    int new_grid[HEIGHT][WIDTH];

    // Initialize the grid with a predefined pattern
    initialize_grid(grid);

    while (1) {
        print_grid(grid);
        update_grid(grid, new_grid);

        // Copy new_grid to grid
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                grid[i][j] = new_grid[i][j];
            }
        }

        // Sleep for a second to slow down the output
        usleep(1000000);
        printf("\033[H\033[J"); // Clear the console
    }

    return 0;
}

// Initialize the grid with a simple pattern (Glider)
void initialize_grid(int grid[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            grid[i][j] = 0; // Set all cells to dead
        }
    }

    // Create a glider pattern
    grid[1][2] = 1;
    grid[2][3] = 1;
    grid[3][1] = 1;
    grid[3][2] = 1;
    grid[3][3] = 1;
}

// Print the grid to the console
void print_grid(int grid[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (grid[i][j] == 1) {
                printf("■ "); // Live cell
            } else {
                printf("□ "); // Dead cell
            }
        }
        printf("\n");
    }
    printf("\n");
}

// Update the grid based on the rules of Conway's Game of Life
void update_grid(int grid[HEIGHT][WIDTH], int new_grid[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            int neighbors = count_neighbors(grid, i, j);
            if (grid[i][j] == 1) {
                // Rule 1 or 3: Any live cell with two or three live neighbors survives
                new_grid[i][j] = (neighbors == 2 || neighbors == 3) ? 1 : 0;
            } else {
                // Rule 4: Any dead cell with exactly three live neighbors becomes a live cell
                new_grid[i][j] = (neighbors == 3) ? 1 : 0;
            }
        }
    }
}

// Count the number of live neighbors around a given cell
int count_neighbors(int grid[HEIGHT][WIDTH], int x, int y) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue; // Skip the cell itself
            int neighbor_x = x + i;
            int neighbor_y = y + j;
            // Check if neighbor is within bounds
            if (neighbor_x >= 0 && neighbor_x < HEIGHT && neighbor_y >= 0 && neighbor_y < WIDTH) {
                count += grid[neighbor_x][neighbor_y];
            }
        }
    }
    return count;
}
