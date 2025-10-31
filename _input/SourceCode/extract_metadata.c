// File: extract_metadata.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

// Function to read metadata from a file
void read_metadata(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Could not open file");
        return;
    }

    char line[MAX_LINE_LENGTH];
    printf("Metadata for file: %s\n", filename);
    
    // Read the file line by line and print the metadata
    while (fgets(line, sizeof(line), file)) {
        // Print the line if it's not empty
        if (strlen(line) > 0) {
            printf("%s", line);
        }
    }

    fclose(file);
}

// Function to extract metadata from a given file type
void extract_metadata(const char *filename) {
    // Check file extension to determine the type
    const char *ext = strrchr(filename, '.');
    if (!ext) {
        printf("File has no extension: %s\n", filename);
        return;
    }

    // Handle different file types
    if (strcmp(ext, ".mp4") == 0 || strcmp(ext, ".mp3") == 0 || strcmp(ext, ".wav") == 0) {
        read_metadata(filename);
    } else {
        printf("Unsupported file type: %s\n", ext);
    }
}

int main(int argc, char *argv[]) {
    // Check command line arguments
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename1> <filename2> ...\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Loop through each file provided in the command line arguments
    for (int i = 1; i < argc; i++) {
        extract_metadata(argv[i]);
    }

    return EXIT_SUCCESS;
}
