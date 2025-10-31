// File: ngram_predictor.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORDS 1000
#define MAX_N 5
#define BUFFER_SIZE 256

typedef struct {
    char *word;
    int count;
} WordCount;

typedef struct {
    WordCount *words;
    int size;
} Ngram;

typedef struct {
    Ngram *ngrams;
    int size;
    int n;
} NgramModel;

// Function to create a new Ngram
Ngram create_ngram() {
    Ngram ngram;
    ngram.words = malloc(MAX_WORDS * sizeof(WordCount));
    ngram.size = 0;
    return ngram;
}

// Function to free an Ngram
void free_ngram(Ngram *ngram) {
    for (int i = 0; i < ngram->size; i++) {
        free(ngram->words[i].word);
    }
    free(ngram->words);
}

// Function to create a new NgramModel
NgramModel create_ngram_model(int n) {
    NgramModel model;
    model.ngrams = malloc(MAX_WORDS * sizeof(Ngram));
    model.size = 0;
    model.n = n;
    return model;
}

// Function to free an NgramModel
void free_ngram_model(NgramModel *model) {
    for (int i = 0; i < model->size; i++) {
        free_ngram(&model->ngrams[i]);
    }
    free(model->ngrams);
}

// Function to find or add a word to an Ngram
void add_word_to_ngram(Ngram *ngram, const char *word) {
    for (int i = 0; i < ngram->size; i++) {
        if (strcmp(ngram->words[i].word, word) == 0) {
            ngram->words[i].count++;
            return;
        }
    }
    ngram->words[ngram->size].word = strdup(word);
    ngram->words[ngram->size].count = 1;
    ngram->size++;
}

// Function to build the n-gram model from text
void build_ngram_model(NgramModel *model, const char *text) {
    char *token;
    char buffer[BUFFER_SIZE];
    char *words[MAX_N];
    int word_count = 0;

    strcpy(buffer, text);
    token = strtok(buffer, " ");
    
    while (token != NULL) {
        words[word_count++] = token;
        if (word_count == model->n) {
            // Create or update the n-gram
            Ngram *ngram = &model->ngrams[model->size];
            model->size++;
            for (int i = 0; i < model->n; i++) {
                add_word_to_ngram(ngram, words[i]);
            }
            word_count--;
            for (int i = 0; i < word_count; i++) {
                words[i] = words[i + 1];
            }
        }
        token = strtok(NULL, " ");
    }
}

// Function to predict the next word based on the n-gram model
void predict_next_word(NgramModel *model, const char *context) {
    char *token;
    char buffer[BUFFER_SIZE];
    char *words[MAX_N];
    int word_count = 0;

    strcpy(buffer, context);
    token = strtok(buffer, " ");
    
    while (token != NULL && word_count < model->n - 1) {
        words[word_count++] = token;
        token = strtok(NULL, " ");
    }

    for (int i = 0; i < model->size; i++) {
        Ngram *ngram = &model->ngrams[i];
        if (ngram->size >= model->n) {
            int match = 1;
            for (int j = 0; j < word_count; j++) {
                if (strcmp(ngram->words[j].word, words[j]) != 0) {
                    match = 0;
                    break;
                }
            }
            if (match) {
                // Print predicted words
                for (int j = 0; j < ngram->size; j++) {
                    printf("%s (%d) ", ngram->words[j].word, ngram->words[j].count);
                }
                printf("\n");
                return;
            }
        }
    }
    printf("No predictions found.\n");
}

int main() {
    NgramModel model = create_ngram_model(3);
    const char *text = "the quick brown fox jumps over the lazy dog the quick";
    
    build_ngram_model(&model, text);
    
    printf("Predicting next word for context 'the quick':\n");
    predict_next_word(&model, "the quick");
    
    free_ngram_model(&model);
    return 0;
}
