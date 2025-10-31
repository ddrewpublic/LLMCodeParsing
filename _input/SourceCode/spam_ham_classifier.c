// File: spam_ham_classifier.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORDS 1000
#define MAX_WORD_LENGTH 100
#define MAX_LINE_LENGTH 1024

typedef struct {
    char word[MAX_WORD_LENGTH];
    int spam_count;
    int ham_count;
} WordCount;

WordCount word_counts[MAX_WORDS];
int total_spam = 0, total_ham = 0;
int unique_words = 0;

void to_lowercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

int find_word(const char *word) {
    for (int i = 0; i < unique_words; i++) {
        if (strcmp(word_counts[i].word, word) == 0) {
            return i;
        }
    }
    return -1;
}

void add_word(const char *word, int is_spam) {
    int index = find_word(word);
    if (index == -1) {
        // New word
        strcpy(word_counts[unique_words].word, word);
        word_counts[unique_words].spam_count = is_spam;
        word_counts[unique_words].ham_count = !is_spam;
        unique_words++;
    } else {
        // Existing word
        if (is_spam) {
            word_counts[index].spam_count++;
        } else {
            word_counts[index].ham_count++;
        }
    }
}

void train(const char *filename, int is_spam) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        // Tokenize the line into words
        char *token = strtok(line, " \n");
        while (token) {
            char word[MAX_WORD_LENGTH];
            to_lowercase(strcpy(word, token));
            add_word(word, is_spam);
            token = strtok(NULL, " \n");
        }
        if (is_spam) {
            total_spam++;
        } else {
            total_ham++;
        }
    }
    fclose(file);
}

double calculate_probability(const char *word, int is_spam) {
    int index = find_word(word);
    if (index == -1) {
        return 1.0; // Unknown word, assume neutral
    }
    double p_word_given_spam = (double)word_counts[index].spam_count / total_spam;
    double p_word_given_ham = (double)word_counts[index].ham_count / total_ham;
    return is_spam ? p_word_given_spam : p_word_given_ham;
}

void classify(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        double p_spam = (double)total_spam / (total_spam + total_ham);
        double p_ham = (double)total_ham / (total_spam + total_ham);

        // Tokenize the line into words
        char *token = strtok(line, " \n");
        while (token) {
            char word[MAX_WORD_LENGTH];
            to_lowercase(strcpy(word, token));
            p_spam *= calculate_probability(word, 1);
            p_ham *= calculate_probability(word, 0);
            token = strtok(NULL, " \n");
        }

        // Classify based on the probabilities
        printf("Message: %s", line);
        if (p_spam > p_ham) {
            printf("Classified as: SPAM\n");
        } else {
            printf("Classified as: HAM\n");
        }
    }
    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <spam_file> <ham_file> <test_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Train the classifier with spam and ham data
    train(argv[1], 1); // Train on spam
    train(argv[2], 0); // Train on ham

    // Classify the test data
    classify(argv[3]);

    return EXIT_SUCCESS;
}
