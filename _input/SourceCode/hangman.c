// File: hangman.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_WORDS 100
#define MAX_WORD_LENGTH 50
#define MAX_TRIES 6

void load_words(char words[MAX_WORDS][MAX_WORD_LENGTH], int *word_count) {
    // Load words from a predefined list
    const char *predefined_words[] = {
        "programming", "hangman", "computer", "science", "challenge",
        "development", "language", "function", "variable", "pointer"
    };
    
    *word_count = sizeof(predefined_words) / sizeof(predefined_words[0]);
    for (int i = 0; i < *word_count; i++) {
        strncpy(words[i], predefined_words[i], MAX_WORD_LENGTH);
    }
}

void choose_word(char *word, const char words[MAX_WORDS][MAX_WORD_LENGTH], int word_count) {
    // Randomly select a word from the list
    srand(time(NULL));
    int index = rand() % word_count;
    strcpy(word, words[index]);
}

void display_progress(const char *word, const char *guessed_letters, int tries) {
    // Display the current progress of the guessed word
    printf("Word: ");
    for (int i = 0; i < strlen(word); i++) {
        if (strchr(guessed_letters, word[i]) != NULL) {
            printf("%c ", word[i]);
        } else {
            printf("_ ");
        }
    }
    printf("\nTries left: %d\n", MAX_TRIES - tries);
}

int main() {
    char words[MAX_WORDS][MAX_WORD_LENGTH];
    int word_count = 0;
    load_words(words, &word_count);

    char word[MAX_WORD_LENGTH];
    choose_word(word, words, word_count);

    char guessed_letters[MAX_WORD_LENGTH] = "";
    int tries = 0;

    printf("Welcome to Hangman!\n");

    while (tries < MAX_TRIES) {
        display_progress(word, guessed_letters, tries);
        
        // Get user input
        char guess;
        printf("Enter a letter: ");
        scanf(" %c", &guess);
        guess = tolower(guess); // Convert to lowercase

        // Check if the letter has already been guessed
        if (strchr(guessed_letters, guess) != NULL) {
            printf("You've already guessed that letter!\n");
            continue;
        }

        // Add the guessed letter to the list
        strncat(guessed_letters, &guess, 1);

        // Check if the guessed letter is in the word
        if (strchr(word, guess) == NULL) {
            tries++;
            printf("Wrong guess!\n");
        }

        // Check if the player has won
        int won = 1;
        for (int i = 0; i < strlen(word); i++) {
            if (strchr(guessed_letters, word[i]) == NULL) {
                won = 0;
                break;
            }
        }

        if (won) {
            printf("Congratulations! You've guessed the word: %s\n", word);
            break;
        }
    }

    if (tries == MAX_TRIES) {
        printf("You've run out of tries! The word was: %s\n", word);
    }

    return 0;
}
