// File: pixel_art_cellular_automata.c

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 80
#define HEIGHT 40
#define ITERATIONS 5
#define THRESHOLD 4

// Function prototypes
void initialize_grid(int grid[HEIGHT][WIDTH]);
void print_grid(int grid[HEIGHT][WIDTH]);
void update_grid(int grid[HEIGHT][WIDTH]);
int count_neighbors(int grid[HEIGHT][WIDTH], int x, int y);

int main() {
    int grid[HEIGHT][WIDTH];

    // Seed the random number generator
    srand(time(NULL));

    // Initialize the grid with random values
    initialize_grid(grid);

    // Generate the pixel art using cellular automata
    for (int i = 0; i < ITERATIONS; i++) {
        update_grid(grid);
    }

    // Print the final grid
    print_grid(grid);

    return 0;
}

// Initialize the grid with random values (0 or 1)
void initialize_grid(int grid[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            grid[y][x] = rand() % 2; // Randomly set to 0 or 1
        }
    }
}

// Print the grid to the console
void print_grid(int grid[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            printf("%c", grid[y][x] ? '#' : ' '); // Print '#' for 1 and ' ' for 0
        }
        printf("\n");
    }
}

// Update the grid based on the cellular automata rules
void update_grid(int grid[HEIGHT][WIDTH]) {
    int new_grid[HEIGHT][WIDTH];

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int neighbors = count_neighbors(grid, x, y);

            // Apply the rules of cellular automata
            if (grid[y][x] == 1) {
                new_grid[y][x] = (neighbors < 2 || neighbors > 3) ? 0 : 1; // Death
            } else {
                new_grid[y][x] = (neighbors == 3) ? 1 : 0; // Birth
            }
        }
    }

    // Copy the new grid to the original grid
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            grid[y][x] = new_grid[y][x];
        }
    }
}

// Count the number of alive neighbors around a cell
int count_neighbors(int grid[HEIGHT][WIDTH], int x, int y) {
    int count = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue; // Skip the cell itself
            int nx = x + dx;
            int ny = y + dy;
            // Check for boundaries
            if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT) {
                count += grid[ny][nx];
            }
        }
    }
    return count;
}
