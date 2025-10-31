// File: markov_poetry_generator.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORDS 1000
#define MAX_WORD_LENGTH 20
#define MAX_LINE_LENGTH 256

typedef struct Node {
    char *word;
    struct Node **next_words;
    int next_word_count;
    int next_word_capacity;
} Node;

typedef struct MarkovChain {
    Node **nodes;
    int node_count;
    int node_capacity;
} MarkovChain;

// Function to create a new node
Node* create_node(const char *word) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->word = strdup(word);
    node->next_word_count = 0;
    node->next_word_capacity = 2;
    node->next_words = (Node **)malloc(node->next_word_capacity * sizeof(Node *));
    return node;
}

// Function to free a node
void free_node(Node *node) {
    free(node->word);
    free(node->next_words);
    free(node);
}

// Function to create a Markov chain
MarkovChain* create_markov_chain() {
    MarkovChain *chain = (MarkovChain *)malloc(sizeof(MarkovChain));
    chain->node_count = 0;
    chain->node_capacity = 2;
    chain->nodes = (Node **)malloc(chain->node_capacity * sizeof(Node *));
    return chain;
}

// Function to find or create a node in the Markov chain
Node* find_or_create_node(MarkovChain *chain, const char *word) {
    for (int i = 0; i < chain->node_count; i++) {
        if (strcmp(chain->nodes[i]->word, word) == 0) {
            return chain->nodes[i];
        }
    }
    if (chain->node_count >= chain->node_capacity) {
        chain->node_capacity *= 2;
        chain->nodes = (Node **)realloc(chain->nodes, chain->node_capacity * sizeof(Node *));
    }
    Node *new_node = create_node(word);
    chain->nodes[chain->node_count++] = new_node;
    return new_node;
}

// Function to add a transition from one word to another
void add_transition(MarkovChain *chain, const char *word1, const char *word2) {
    Node *node1 = find_or_create_node(chain, word1);
    if (node1->next_word_count >= node1->next_word_capacity) {
        node1->next_word_capacity *= 2;
        node1->next_words = (Node **)realloc(node1->next_words, node1->next_word_capacity * sizeof(Node *));
    }
    Node *node2 = find_or_create_node(chain, word2);
    node1->next_words[node1->next_word_count++] = node2;
}

// Function to generate a random line of poetry
void generate_poetry_line(MarkovChain *chain, int max_words) {
    if (chain->node_count == 0) return;

    Node *current_node = chain->nodes[rand() % chain->node_count];
    for (int i = 0; i < max_words; i++) {
        printf("%s ", current_node->word);
        if (current_node->next_word_count == 0) break;
        current_node = current_node->next_words[rand() % current_node->next_word_count];
    }
    printf("\n");
}

// Function to free the Markov chain
void free_markov_chain(MarkovChain *chain) {
    for (int i = 0; i < chain->node_count; i++) {
        free_node(chain->nodes[i]);
    }
    free(chain->nodes);
    free(chain);
}

// Main function to read input and generate poetry
int main() {
    srand((unsigned int)time(NULL));

    MarkovChain *chain = create_markov_chain();
    char line[MAX_LINE_LENGTH];
    
    // Read input lines and build the Markov chain
    printf("Enter lines of text (Ctrl+D to end):\n");
    while (fgets(line, sizeof(line), stdin)) {
        char *token = strtok(line, " \n");
        char *prev_word = NULL;
        while (token) {
            if (prev_word) {
                add_transition(chain, prev_word, token);
            }
            prev_word = token;
            token = strtok(NULL, " \n");
        }
    }

    // Generate poetry
    printf("\nGenerated Poetry:\n");
    for (int i = 0; i < 5; i++) {
        generate_poetry_line(chain, 10);
    }

    // Free resources
    free_markov_chain(chain);
    return 0;
}
