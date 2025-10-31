// File: mad_libs_generator.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORDS 100
#define MAX_LENGTH 100

// Function prototypes
void get_input(char *prompt, char *input);
void generate_story(const char *template, char words[][MAX_LENGTH], int word_count);

int main() {
    char template[MAX_LENGTH];
    char words[MAX_WORDS][MAX_LENGTH];
    int word_count = 0;
    
    // Get the story template from the user
    printf("Enter a story template (use placeholders like {noun}, {verb}, {adjective}):\n");
    fgets(template, sizeof(template), stdin);
    template[strcspn(template, "\n")] = 0; // Remove newline character

    // Get words from the user
    char *placeholders[] = {"noun", "verb", "adjective", "adverb"};
    int num_placeholders = sizeof(placeholders) / sizeof(placeholders[0]);

    for (int i = 0; i < num_placeholders; i++) {
        get_input("Enter a ", words[word_count]);
        word_count++;
    }

    // Generate and display the story
    generate_story(template, words, word_count);

    return 0;
}

// Function to get user input
void get_input(char *prompt, char *input) {
    printf("%s%s: ", prompt, input);
    fgets(input, MAX_LENGTH, stdin);
    input[strcspn(input, "\n")] = 0; // Remove newline character
}

// Function to generate the story based on the template and user inputs
void generate_story(const char *template, char words[][MAX_LENGTH], int word_count) {
    char story[MAX_LENGTH * 10]; // Allocate enough space for the story
    strcpy(story, template); // Start with the template

    // Replace placeholders with user input
    for (int i = 0; i < word_count; i++) {
        char placeholder[MAX_LENGTH];
        snprintf(placeholder, sizeof(placeholder), "{%s}", (i % 4 == 0) ? "noun" : (i % 4 == 1) ? "verb" : (i % 4 == 2) ? "adjective" : "adverb");

        char *pos = strstr(story, placeholder);
        while (pos != NULL) {
            // Create a new story with the replaced word
            char temp[MAX_LENGTH * 10];
            strncpy(temp, story, pos - story);
            temp[pos - story] = '\0';
            strcat(temp, words[i]);
            strcat(temp, pos + strlen(placeholder));
            strcpy(story, temp);
            pos = strstr(story, placeholder); // Find the next occurrence
        }
    }

    // Print the final story
    printf("\nHere is your story:\n%s\n", story);
}
