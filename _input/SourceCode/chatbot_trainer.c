// File: chatbot_trainer.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RESPONSES 100
#define MAX_LENGTH 256

typedef struct {
    char input[MAX_LENGTH];
    char response[MAX_LENGTH];
} ChatbotEntry;

ChatbotEntry dataset[MAX_RESPONSES];
int entry_count = 0;

// Function to load dataset from a file
void load_dataset(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open dataset file");
        exit(EXIT_FAILURE);
    }

    while (fscanf(file, "%255[^\n]\n%255[^\n]\n", dataset[entry_count].input, dataset[entry_count].response) == 2) {
        entry_count++;
        if (entry_count >= MAX_RESPONSES) {
            break;
        }
    }

    fclose(file);
}

// Function to find a response based on user input
const char* get_response(const char *user_input) {
    for (int i = 0; i < entry_count; i++) {
        if (strcmp(dataset[i].input, user_input) == 0) {
            return dataset[i].response;
        }
    }
    return "I'm sorry, I don't understand.";
}

// Function to interact with the user
void chat() {
    char user_input[MAX_LENGTH];

    printf("Chatbot: Hello! How can I assist you today?\n");
    while (1) {
        printf("You: ");
        fgets(user_input, MAX_LENGTH, stdin);
        user_input[strcspn(user_input, "\n")] = 0; // Remove newline character

        if (strcmp(user_input, "exit") == 0) {
            printf("Chatbot: Goodbye!\n");
            break;
        }

        const char *response = get_response(user_input);
        printf("Chatbot: %s\n", response);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <dataset_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    load_dataset(argv[1]);
    chat();

    return EXIT_SUCCESS;
}
