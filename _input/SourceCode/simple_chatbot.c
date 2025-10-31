// File: simple_chatbot.c

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT_LENGTH 256
#define MAX_KEYWORDS 10
#define MAX_RESPONSE_LENGTH 512

typedef struct {
    char keyword[MAX_INPUT_LENGTH];
    char response[MAX_RESPONSE_LENGTH];
} KeywordResponse;

// Function to convert a string to lowercase
void to_lowercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

// Function to check if the input contains a keyword and return the corresponding response
const char* get_response(const char *input, KeywordResponse *responses, int num_keywords) {
    for (int i = 0; i < num_keywords; i++) {
        if (strstr(input, responses[i].keyword) != NULL) {
            return responses[i].response;
        }
    }
    return "I'm sorry, I don't understand.";
}

int main() {
    // Define keywords and their corresponding responses
    KeywordResponse responses[MAX_KEYWORDS] = {
        {"hello", "Hi there! How can I help you today?"},
        {"how are you", "I'm just a program, but thanks for asking!"},
        {"bye", "Goodbye! Have a great day!"},
        {"help", "Sure! What do you need help with?"},
        {"your name", "I am a simple chatbot."},
        {"weather", "I can't check the weather, but I hope it's nice!"},
        {"joke", "Why don't scientists trust atoms? Because they make up everything!"},
        {"news", "I don't have real-time news, but I can chat!"},
        {"time", "I can't tell time, but it's always a good time to chat!"},
        {"love", "Love is a beautiful thing!"}
    };
    
    int num_keywords = 10;
    char input[MAX_INPUT_LENGTH];

    printf("Welcome to the Chatbot! Type 'bye' to exit.\n");

    // Main loop to get user input and respond
    while (1) {
        printf("You: ");
        fgets(input, sizeof(input), stdin);
        
        // Remove newline character from input
        input[strcspn(input, "\n")] = 0;

        // Convert input to lowercase for case-insensitive matching
        to_lowercase(input);

        // Get the response based on the input
        const char *response = get_response(input, responses, num_keywords);
        printf("Chatbot: %s\n", response);

        // Exit the loop if the user says 'bye'
        if (strcmp(input, "bye") == 0) {
            break;
        }
    }

    printf("Chatbot: Goodbye! Have a great day!\n");
    return 0;
}
