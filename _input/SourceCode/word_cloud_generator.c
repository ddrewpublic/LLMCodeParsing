// File: word_cloud_generator.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LENGTH 100
#define MAX_WORDS 1000

typedef struct {
    char word[MAX_WORD_LENGTH];
    int count;
} WordCount;

WordCount wordCounts[MAX_WORDS];
int wordCountSize = 0;

// Function to convert a string to lowercase
void toLowerCase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

// Function to check if a word exists in the wordCounts array
int findWord(const char *word) {
    for (int i = 0; i < wordCountSize; i++) {
        if (strcmp(wordCounts[i].word, word) == 0) {
            return i; // Return index if found
        }
    }
    return -1; // Not found
}

// Function to add a word to the wordCounts array
void addWord(const char *word) {
    int index = findWord(word);
    if (index != -1) {
        wordCounts[index].count++; // Increment count if word exists
    } else {
        if (wordCountSize < MAX_WORDS) {
            strcpy(wordCounts[wordCountSize].word, word);
            wordCounts[wordCountSize].count = 1; // Initialize count to 1
            wordCountSize++;
        }
    }
}

// Function to read words from a file and populate the wordCounts array
void readFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    char word[MAX_WORD_LENGTH];
    while (fscanf(file, "%99s", word) == 1) {
        toLowerCase(word); // Convert word to lowercase
        // Remove punctuation from the word
        char *p = word;
        while (*p) {
            if (ispunct((unsigned char)*p)) {
                *p = '\0'; // Terminate the string at the first punctuation
                break;
            }
            p++;
        }
        if (strlen(word) > 0) {
            addWord(word); // Add the cleaned word to the counts
        }
    }
    fclose(file);
}

// Function to compare two WordCount structures for sorting
int compare(const void *a, const void *b) {
    return ((WordCount *)b)->count - ((WordCount *)a)->count; // Sort in descending order
}

// Function to print the word cloud
void printWordCloud() {
    for (int i = 0; i < wordCountSize; i++) {
        printf("%s: %d\n", wordCounts[i].word, wordCounts[i].count);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    readFile(argv[1]); // Read words from the specified file
    qsort(wordCounts, wordCountSize, sizeof(WordCount), compare); // Sort the words by frequency
    printWordCloud(); // Print the word cloud

    return EXIT_SUCCESS;
}
