// File: word_frequency_counter.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LENGTH 100
#define INITIAL_CAPACITY 10

typedef struct {
    char *word;
    int count;
} WordCount;

typedef struct {
    WordCount *array;
    int size;
    int capacity;
} WordCountList;

// Function to create a new WordCountList
WordCountList* create_word_count_list() {
    WordCountList *list = malloc(sizeof(WordCountList));
    list->size = 0;
    list->capacity = INITIAL_CAPACITY;
    list->array = malloc(list->capacity * sizeof(WordCount));
    return list;
}

// Function to free the WordCountList
void free_word_count_list(WordCountList *list) {
    for (int i = 0; i < list->size; i++) {
        free(list->array[i].word);
    }
    free(list->array);
    free(list);
}

// Function to add or update a word in the list
void add_word(WordCountList *list, const char *word) {
    // Check if we need to increase the capacity
    if (list->size >= list->capacity) {
        list->capacity *= 2;
        list->array = realloc(list->array, list->capacity * sizeof(WordCount));
    }

    // Search for the word in the list
    for (int i = 0; i < list->size; i++) {
        if (strcmp(list->array[i].word, word) == 0) {
            list->array[i].count++;
            return;
        }
    }

    // If the word is not found, add it to the list
    list->array[list->size].word = strdup(word);
    list->array[list->size].count = 1;
    list->size++;
}

// Function to compare two WordCount objects for qsort
int compare_word_count(const void *a, const void *b) {
    return ((WordCount *)b)->count - ((WordCount *)a)->count; // Sort by count descending
}

// Function to read words from a file and count their frequencies
void count_word_frequencies(FILE *file, WordCountList *list) {
    char word[MAX_WORD_LENGTH];
    while (fscanf(file, "%99s", word) == 1) {
        // Normalize the word to lowercase and remove punctuation
        for (char *p = word; *p; p++) {
            *p = tolower(*p);
            if (!isalpha(*p) && *p != '\'') {
                *p = '\0'; // Terminate the string at the first non-alpha character
                break;
            }
        }
        if (strlen(word) > 0) {
            add_word(list, word);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    WordCountList *list = create_word_count_list();
    count_word_frequencies(file, list);
    fclose(file);

    // Sort the word counts in descending order
    qsort(list->array, list->size, sizeof(WordCount), compare_word_count);

    // Print the word frequencies
    for (int i = 0; i < list->size; i++) {
        printf("%s: %d\n", list->array[i].word, list->array[i].count);
    }

    free_word_count_list(list);
    return EXIT_SUCCESS;
}
