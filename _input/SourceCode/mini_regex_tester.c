// File: mini_regex_tester.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Function prototypes
bool match(const char *pattern, const char *text);
bool match_here(const char *pattern, const char *text);
bool match_star(char c, const char *pattern, const char *text);

// Main function
int main() {
    char pattern[100];
    char text[100];

    // Get user input for pattern and text
    printf("Enter a regular expression pattern: ");
    scanf("%99s", pattern);
    printf("Enter a string to match: ");
    scanf("%99s", text);

    // Perform matching
    if (match(pattern, text)) {
        printf("Match found!\n");
    } else {
        printf("No match.\n");
    }

    return 0;
}

// Function to match a pattern against a text
bool match(const char *pattern, const char *text) {
    // Start matching from the beginning of both pattern and text
    return match_here(pattern, text);
}

// Function to match the pattern at the current position in the text
bool match_here(const char *pattern, const char *text) {
    // If the pattern is empty, we've matched the entire text
    if (*pattern == '\0') {
        return *text == '\0';
    }

    // Check for '*' in the pattern
    if (*(pattern + 1) == '*') {
        // Match zero or more of the preceding element
        return match_star(*pattern, pattern + 2, text);
    }

    // If the current characters match, move to the next character
    if (*text != '\0' && (*text == *pattern || *pattern == '.')) {
        return match_here(pattern + 1, text + 1);
    }

    // No match
    return false;
}

// Function to handle '*' in the pattern
bool match_star(char c, const char *pattern, const char *text) {
    // Try to match zero or more occurrences of c
    do {
        // Check if the rest of the pattern matches
        if (match_here(pattern, text)) {
            return true;
        }
    } while (*text != '\0' && (*text++ == c || c == '.'));

    // No match found
    return false;
}
