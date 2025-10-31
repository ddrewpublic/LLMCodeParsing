// File: csv_to_json_converter.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define MAX_FIELDS 100

// Function to trim whitespace from both ends of a string
char* trim_whitespace(char* str) {
    char* end;

    // Trim leading space
    while (*str == ' ') str++;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && *end == ' ') end--;

    // Null terminate after the last non-space character
    *(end + 1) = '\0';
    return str;
}

// Function to convert CSV to JSON
void convert_csv_to_json(FILE* csv_file, FILE* json_file) {
    char line[MAX_LINE_LENGTH];
    char* fields[MAX_FIELDS];
    char* token;
    int is_first_line = 1;
    int field_count = 0;

    fprintf(json_file, "[\n");

    // Read the first line to get the headers
    if (fgets(line, sizeof(line), csv_file)) {
        token = strtok(line, ",");
        while (token) {
            fields[field_count++] = trim_whitespace(token);
            token = strtok(NULL, ",");
        }
    }

    // Read the rest of the lines
    while (fgets(line, sizeof(line), csv_file)) {
        if (!is_first_line) {
            fprintf(json_file, ",\n");
        }
        is_first_line = 0;

        fprintf(json_file, "  {");
        token = strtok(line, ",");
        for (int i = 0; i < field_count; i++) {
            if (i > 0) {
                fprintf(json_file, ", ");
            }
            char* value = trim_whitespace(token);
            fprintf(json_file, "\"%s\": \"%s\"", fields[i], value);
            token = strtok(NULL, ",");
        }
        fprintf(json_file, "}");
    }

    fprintf(json_file, "\n]\n");
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.csv> <output.json>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE* csv_file = fopen(argv[1], "r");
    if (!csv_file) {
        perror("Error opening CSV file");
        return EXIT_FAILURE;
    }

    FILE* json_file = fopen(argv[2], "w");
    if (!json_file) {
        perror("Error opening JSON file");
        fclose(csv_file);
        return EXIT_FAILURE;
    }

    convert_csv_to_json(csv_file, json_file);

    fclose(csv_file);
    fclose(json_file);

    return EXIT_SUCCESS;
}
