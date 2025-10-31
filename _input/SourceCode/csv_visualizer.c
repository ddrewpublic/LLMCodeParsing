// File: csv_visualizer.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define MAX_COLUMNS 10

// Function to read the CSV file and store data
int read_csv(const char *filename, char data[][MAX_COLUMNS][MAX_LINE_LENGTH], int *row_count, int *col_count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Could not open file");
        return -1;
    }

    char line[MAX_LINE_LENGTH];
    *row_count = 0;
    *col_count = 0;

    // Read each line from the CSV file
    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",");
        int col_index = 0;

        while (token != NULL && col_index < MAX_COLUMNS) {
            strcpy(data[*row_count][col_index], token);
            token = strtok(NULL, ",");
            col_index++;
        }

        if (col_index > *col_count) {
            *col_count = col_index; // Update column count
        }
        (*row_count)++;
    }

    fclose(file);
    return 0;
}

// Function to visualize data as a simple text-based bar chart
void visualize_data(char data[][MAX_COLUMNS][MAX_LINE_LENGTH], int row_count, int col_count) {
    printf("\nData Visualization:\n");

    for (int i = 1; i < row_count; i++) { // Skip header
        printf("%s: ", data[i][0]); // Assuming first column is the label
        int value = atoi(data[i][1]); // Assuming second column is the value
        for (int j = 0; j < value; j++) {
            printf("#");
        }
        printf(" (%d)\n", value);
    }
}

// Main function
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <csv_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char data[MAX_LINE_LENGTH][MAX_COLUMNS][MAX_LINE_LENGTH];
    int row_count = 0, col_count = 0;

    // Read CSV data
    if (read_csv(argv[1], data, &row_count, &col_count) != 0) {
        return EXIT_FAILURE;
    }

    // Visualize the data
    visualize_data(data, row_count, col_count);
    
    return EXIT_SUCCESS;
}
