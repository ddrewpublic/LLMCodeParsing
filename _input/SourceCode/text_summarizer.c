// File: text_summarizer.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1024
#define MAX_WORDS 10000
#define MAX_WORD_LENGTH 100

typedef struct {
    char word[MAX_WORD_LENGTH];
    int count;
} WordCount;

// Function prototypes
void to_lowercase(char *str);
int is_punctuation(char c);
void add_word(WordCount *word_counts, int *word_count, const char *word);
int compare(const void *a, const void *b);
void summarize_text(const char *filename);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }
    summarize_text(argv[1]);
    return EXIT_SUCCESS;
}

// Convert a string to lowercase
void to_lowercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

// Check if a character is punctuation
int is_punctuation(char c) {
    return ispunct(c) || isspace(c);
}

// Add a word to the word count array
void add_word(WordCount *word_counts, int *word_count, const char *word) {
    for (int i = 0; i < *word_count; i++) {
        if (strcmp(word_counts[i].word, word) == 0) {
            word_counts[i].count++;
            return;
        }
    }
    // If word not found, add it
    strcpy(word_counts[*word_count].word, word);
    word_counts[*word_count].count = 1;
    (*word_count)++;
}

// Comparison function for qsort
int compare(const void *a, const void *b) {
    WordCount *wc_a = (WordCount *)a;
    WordCount *wc_b = (WordCount *)b;
    return wc_b->count - wc_a->count; // Sort in descending order
}

// Summarize the text by counting word frequencies
void summarize_text(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Could not open file");
        return;
    }

    WordCount word_counts[MAX_WORDS];
    int word_count = 0;
    char line[MAX_LINE_LENGTH];

    // Read the file line by line
    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, " ");
        while (token) {
            // Remove punctuation from the token
            char cleaned_word[MAX_WORD_LENGTH] = {0};
            int j = 0;
            for (int i = 0; token[i]; i++) {
                if (!is_punctuation(token[i])) {
                    cleaned_word[j++] = token[i];
                }
            }
            cleaned_word[j] = '\0';
            to_lowercase(cleaned_word); // Convert to lowercase

            if (strlen(cleaned_word) > 0) {
                add_word(word_counts, &word_count, cleaned_word);
            }
            token = strtok(NULL, " ");
        }
    }

    fclose(file);

    // Sort the word counts
    qsort(word_counts, word_count, sizeof(WordCount), compare);

    // Print the top 10 words
    printf("Top 10 words:\n");
    for (int i = 0; i < word_count && i < 10; i++) {
        printf("%s: %d\n", word_counts[i].word, word_counts[i].count);
    }
}
