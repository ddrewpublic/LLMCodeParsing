// File: grammar_checker.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1024
#define MAX_WORD_LENGTH 100
#define MAX_WORDS 200

// Function prototypes
void to_lower_case(char *str);
int is_article(char *word);
int is_preposition(char *word);
int is_conjunction(char *word);
void correct_grammar(char *line);
void process_line(char *line);

const char *articles[] = {"a", "an", "the", NULL};
const char *prepositions[] = {"in", "on", "at", "for", "with", "about", "against", "between", "into", "through", NULL};
const char *conjunctions[] = {"and", "but", "or", "nor", "for", "so", "yet", NULL};

// Convert a string to lowercase
void to_lower_case(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

// Check if a word is an article
int is_article(char *word) {
    for (int i = 0; articles[i] != NULL; i++) {
        if (strcmp(word, articles[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Check if a word is a preposition
int is_preposition(char *word) {
    for (int i = 0; prepositions[i] != NULL; i++) {
        if (strcmp(word, prepositions[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Check if a word is a conjunction
int is_conjunction(char *word) {
    for (int i = 0; conjunctions[i] != NULL; i++) {
        if (strcmp(word, conjunctions[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Correct common grammar errors in a line
void correct_grammar(char *line) {
    char *token;
    char corrected_line[MAX_LINE_LENGTH] = "";
    char prev_word[MAX_WORD_LENGTH] = "";
    
    token = strtok(line, " ");
    while (token != NULL) {
        char current_word[MAX_WORD_LENGTH];
        strcpy(current_word, token);
        to_lower_case(current_word);

        // Check for article usage
        if (is_article(current_word) && strlen(prev_word) > 0 && is_preposition(prev_word)) {
            // Incorrect article after a preposition
            strcat(corrected_line, "the "); // Suggest correction
        } else if (is_conjunction(current_word) && strlen(prev_word) == 0) {
            // Incorrect conjunction at the start
            strcat(corrected_line, " "); // Skip
        } else {
            strcat(corrected_line, token);
            strcat(corrected_line, " ");
        }

        strcpy(prev_word, current_word);
        token = strtok(NULL, " ");
    }

    // Output the corrected line
    printf("Corrected: %s\n", corrected_line);
}

// Process each line of input
void process_line(char *line) {
    printf("Original: %s\n", line);
    correct_grammar(line);
}

int main() {
    char line[MAX_LINE_LENGTH];

    printf("Enter text (type 'exit' to quit):\n");
    while (1) {
        // Read a line of input
        if (!fgets(line, sizeof(line), stdin)) {
            break;
        }
        
        // Remove newline character
        line[strcspn(line, "\n")] = 0;

        // Exit condition
        if (strcmp(line, "exit") == 0) {
            break;
        }

        // Process the line for grammar checking
        process_line(line);
    }

    return 0;
}
