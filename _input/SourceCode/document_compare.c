// File: document_compare.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define MAX_LINES 1000

// Function to read lines from a file into an array
int read_lines(const char *filename, char lines[MAX_LINES][MAX_LINE_LENGTH]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Could not open file");
        return -1;
    }

    int count = 0;
    while (fgets(lines[count], MAX_LINE_LENGTH, file) && count < MAX_LINES) {
        count++;
    }

    fclose(file);
    return count;
}

// Function to compare two arrays of lines and print differences
void compare_lines(char lines1[MAX_LINES][MAX_LINE_LENGTH], int count1,
                   char lines2[MAX_LINES][MAX_LINE_LENGTH], int count2) {
    int i = 0, j = 0;

    while (i < count1 || j < count2) {
        if (i < count1 && j < count2) {
            if (strcmp(lines1[i], lines2[j]) == 0) {
                // Lines are the same, print nothing
                i++;
                j++;
            } else {
                // Lines are different, highlight the differences
                printf("Difference found:\n");
                printf("< %s", lines1[i]);
                printf("> %s", lines2[j]);
                i++;
                j++;
            }
        } else if (i < count1) {
            // Only lines from the first document
            printf("Only in first document:\n");
            printf("< %s", lines1[i]);
            i++;
        } else if (j < count2) {
            // Only lines from the second document
            printf("Only in second document:\n");
            printf("> %s", lines2[j]);
            j++;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <file1> <file2>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char lines1[MAX_LINES][MAX_LINE_LENGTH];
    char lines2[MAX_LINES][MAX_LINE_LENGTH];

    int count1 = read_lines(argv[1], lines1);
    if (count1 < 0) return EXIT_FAILURE;

    int count2 = read_lines(argv[2], lines2);
    if (count2 < 0) return EXIT_FAILURE;

    compare_lines(lines1, count1, lines2, count2);

    return EXIT_SUCCESS;
}
