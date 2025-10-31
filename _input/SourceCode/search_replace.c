// File: search_replace.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

// Function to replace all occurrences of a substring in a string
char *replace_substring(const char *str, const char *old, const char *new) {
    char *result;
    int i, count = 0;
    int newlen = strlen(new);
    int oldlen = strlen(old);

    // Count the number of times old substring occurs in str
    for (i = 0; str[i] != '\0'; i++) {
        if (strstr(&str[i], old) == &str[i]) {
            count++;
            i += oldlen - 1; // Move past the old substring
        }
    }

    // Allocate memory for the new result string
    result = (char *)malloc(i + count * (newlen - oldlen) + 1);
    if (!result) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    i = 0;
    while (*str) {
        // Compare the substring with the result
        if (strstr(str, old) == str) {
            strcpy(&result[i], new);
            i += newlen;
            str += oldlen;
        } else {
            result[i++] = *str++;
        }
    }
    result[i] = '\0';
    return result;
}

// Function to read a file and replace patterns
void search_and_replace(const char *filename, const char *old, const char *new) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Could not open file %s for reading\n", filename);
        return;
    }

    // Read the entire file into a buffer
    char *buffer = (char *)malloc(BUFFER_SIZE);
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return;
    }

    size_t total_size = 0;
    size_t bytes_read;
    while ((bytes_read = fread(buffer + total_size, 1, BUFFER_SIZE - total_size, file)) > 0) {
        total_size += bytes_read;
    }
    buffer[total_size] = '\0';
    fclose(file);

    // Replace the old substring with the new one
    char *result = replace_substring(buffer, old, new);
    free(buffer);

    // Write the result back to the file
    file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Could not open file %s for writing\n", filename);
        free(result);
        return;
    }
    fwrite(result, sizeof(char), strlen(result), file);
    fclose(file);
    free(result);
}

// Main function to handle command line arguments and perform search and replace
int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <filename> <old_pattern> <new_pattern>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    const char *old_pattern = argv[2];
    const char *new_pattern = argv[3];

    search_and_replace(filename, old_pattern, new_pattern);
    printf("Replaced '%s' with '%s' in file '%s'\n", old_pattern, new_pattern, filename);

    return EXIT_SUCCESS;
}
