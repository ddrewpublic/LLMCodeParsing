// File: yaml_json_converter.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function prototypes
void convert_yaml_to_json(const char *yaml, char *json);
void convert_json_to_yaml(const char *json, char *yaml);
void trim_whitespace(char *str);
int is_json(const char *str);
int is_yaml(const char *str);

// Main function
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Read input file
    FILE *input_file = fopen(argv[1], "r");
    if (!input_file) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }
    
    fseek(input_file, 0, SEEK_END);
    long input_length = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);
    
    char *input_buffer = malloc(input_length + 1);
    if (!input_buffer) {
        perror("Error allocating memory");
        fclose(input_file);
        return EXIT_FAILURE;
    }
    
    fread(input_buffer, 1, input_length, input_file);
    input_buffer[input_length] = '\0';
    fclose(input_file);

    // Determine if the input is YAML or JSON
    char *output_buffer = malloc(input_length * 2); // Allocate more for output
    if (!output_buffer) {
        perror("Error allocating memory");
        free(input_buffer);
        return EXIT_FAILURE;
    }
    
    if (is_yaml(input_buffer)) {
        convert_yaml_to_json(input_buffer, output_buffer);
    } else if (is_json(input_buffer)) {
        convert_json_to_yaml(input_buffer, output_buffer);
    } else {
        fprintf(stderr, "Input format not recognized.\n");
        free(input_buffer);
        free(output_buffer);
        return EXIT_FAILURE;
    }

    // Write output file
    FILE *output_file = fopen(argv[2], "w");
    if (!output_file) {
        perror("Error opening output file");
        free(input_buffer);
        free(output_buffer);
        return EXIT_FAILURE;
    }

    fputs(output_buffer, output_file);
    fclose(output_file);

    // Clean up
    free(input_buffer);
    free(output_buffer);
    
    return EXIT_SUCCESS;
}

// Convert YAML to JSON (simple implementation)
void convert_yaml_to_json(const char *yaml, char *json) {
    // This is a simplified example and does not cover all YAML features
    strcpy(json, "{"); // Start JSON object
    char *line = strtok((char *)yaml, "\n");
    while (line) {
        trim_whitespace(line);
        if (strlen(line) > 0) {
            char *key = strtok(line, ":");
            char *value = strtok(NULL, ":");
            if (key && value) {
                trim_whitespace(key);
                trim_whitespace(value);
                sprintf(json + strlen(json), "\"%s\": \"%s\", ", key, value);
            }
        }
        line = strtok(NULL, "\n");
    }
    // Remove trailing comma and space
    if (strlen(json) > 1) {
        json[strlen(json) - 2] = '\0'; // Remove last comma
    }
    strcat(json, "}"); // End JSON object
}

// Convert JSON to YAML (simple implementation)
void convert_json_to_yaml(const char *json, char *yaml) {
    // This is a simplified example and does not cover all JSON features
    strcpy(yaml, ""); // Start with empty YAML
    char *key_value = strtok((char *)json + 1, ",}"); // Skip '{' and split by ','
    while (key_value) {
        char *key = strtok(key_value, ":");
        char *value = strtok(NULL, ":");
        if (key && value) {
            trim_whitespace(key);
            trim_whitespace(value);
            sprintf(yaml + strlen(yaml), "%s: %s\n", key + 1, value + 1); // Remove quotes
        }
        key_value = strtok(NULL, ",}");
    }
}

// Trim leading and trailing whitespace
void trim_whitespace(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return; // All spaces
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
}

// Check if the string is JSON
int is_json(const char *str) {
    return str[0] == '{' || str[0] == '['; // Basic check
}

// Check if the string is YAML
int is_yaml(const char *str) {
    return strchr(str, ':') != NULL; // Basic check
}
