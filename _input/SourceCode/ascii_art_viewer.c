// File: ascii_art_viewer.c
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 1024

// Function to display the ASCII art from a file
void display_ascii_art(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    // Read each line from the file and print it to the console
    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    // Check if the user provided a filename as an argument
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Display the ASCII art from the provided file
    display_ascii_art(argv[1]);

    return 0;
}
