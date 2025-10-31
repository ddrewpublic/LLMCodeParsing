// File: html_to_plain_text.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

// Function to check if a character is a whitespace
int is_whitespace(char c) {
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

// Function to convert HTML to plain text
void convert_html_to_text(FILE *input, FILE *output) {
    char buffer[BUFFER_SIZE];
    int in_tag = 0; // Flag to indicate if we are inside an HTML tag
    int in_comment = 0; // Flag to indicate if we are inside an HTML comment
    int in_script = 0; // Flag to indicate if we are inside a <script> tag
    int in_style = 0; // Flag to indicate if we are inside a <style> tag

    while (fgets(buffer, BUFFER_SIZE, input) != NULL) {
        for (char *p = buffer; *p != '\0'; p++) {
            // Check for the start of a tag
            if (*p == '<') {
                // Check for comments
                if (strncmp(p, "<!--", 4) == 0) {
                    in_comment = 1;
                    p += 3; // Move pointer to the end of comment start
                    continue;
                }
                // Check for script or style tags
                if (strncasecmp(p, "<script", 7) == 0) {
                    in_script = 1;
                    p += 6; // Move pointer to the end of <script>
                    continue;
                }
                if (strncasecmp(p, "<style", 6) == 0) {
                    in_style = 1;
                    p += 5; // Move pointer to the end of <style>
                    continue;
                }
                in_tag = 1; // We are now inside a tag
            } else if (*p == '>') {
                // End of a tag
                in_tag = 0;
                in_comment = 0;
                in_script = 0;
                in_style = 0;
                continue;
            }

            // If we are inside a comment, script, or style, skip the content
            if (in_comment || in_script || in_style) {
                continue;
            }

            // If not in a tag, print the character
            if (!in_tag) {
                // Print only if it's not a whitespace character
                if (!is_whitespace(*p)) {
                    fputc(*p, output);
                } else {
                    // Print a single space for consecutive whitespace
                    if (p > buffer && !is_whitespace(*(p - 1))) {
                        fputc(' ', output);
                    }
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.html> <output.txt>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    FILE *output = fopen(argv[2], "w");
    if (!output) {
        perror("Error opening output file");
        fclose(input);
        return EXIT_FAILURE;
    }

    convert_html_to_text(input, output);

    fclose(input);
    fclose(output);

    return EXIT_SUCCESS;
}
