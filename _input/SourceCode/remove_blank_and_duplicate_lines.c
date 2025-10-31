// File: remove_blank_and_duplicate_lines.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 1024
#define MAX_LINES 10000

// Function to trim whitespace from both ends of a string
char* trim_whitespace(char* str) {
    char *end;

    // Trim leading space
    while (*str == ' ') str++;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\n' || *end == '\r')) end--;
    
    // Null terminate the string after the last non-whitespace character
    *(end + 1) = '\0';

    return str;
}

// Function to check if two lines are duplicates (ignoring whitespace)
bool are_lines_duplicate(char* line1, char* line2) {
    char* trimmed_line1 = trim_whitespace(line1);
    char* trimmed_line2 = trim_whitespace(line2);
    return strcmp(trimmed_line1, trimmed_line2) == 0;
}

// Function to read lines from a file and store unique non-blank lines
int read_unique_lines(const char* filename, char lines[MAX_LINES][MAX_LINE_LENGTH]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return -1;
    }

    char line[MAX_LINE_LENGTH];
    int line_count = 0;

    while (fgets(line, sizeof(line), file)) {
        char *trimmed_line = trim_whitespace(line);
        
        // Skip blank lines
        if (strlen(trimmed_line) == 0) {
            continue;
        }

        // Check for duplicates
        bool is_duplicate = false;
        for (int i = 0; i < line_count; i++) {
            if (are_lines_duplicate(lines[i], trimmed_line)) {
                is_duplicate = true;
                break;
            }
        }

        // If not a duplicate, store the line
        if (!is_duplicate && line_count < MAX_LINES) {
            strcpy(lines[line_count++], trimmed_line);
        }
    }

    fclose(file);
    return line_count;
}

// Function to write the unique lines back to the file
void write_lines_to_file(const char* filename, char lines[MAX_LINES][MAX_LINE_LENGTH], int line_count) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }

    for (int i = 0; i < line_count; i++) {
        fprintf(file, "%s\n", lines[i]);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char lines[MAX_LINES][MAX_LINE_LENGTH];
    int unique_line_count = read_unique_lines(argv[1], lines);
    if (unique_line_count < 0) {
        return EXIT_FAILURE;
    }

    write_lines_to_file(argv[1], lines, unique_line_count);
    printf("Processed file: %s\n", argv[1]);
    printf("Removed blank and duplicate lines. Remaining lines: %d\n", unique_line_count);

    return EXIT_SUCCESS;
}
