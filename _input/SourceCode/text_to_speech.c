// File: text_to_speech.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// Function to convert text to speech using system command
void text_to_speech(const char *text) {
    // Create a temporary file to store the text
    FILE *fp = fopen("temp.txt", "w");
    if (fp == NULL) {
        perror("Failed to create temporary file");
        return;
    }

    // Write the user input to the temporary file
    fprintf(fp, "%s", text);
    fclose(fp);

#ifdef _WIN32
    // Use the built-in SAPI on Windows to read the text aloud
    char command[256];
    snprintf(command, sizeof(command), "powershell -Command \"Add-Type –AssemblyName System.Speech; " \
                                       "$speak = New-Object System.Speech.Synthesis.SpeechSynthesizer; " \
                                       "$speak.Speak([System.IO.File]::ReadAllText('temp.txt'))\"");
    system(command);
#else
    // Use espeak on Linux to read the text aloud
    char command[256];
    snprintf(command, sizeof(command), "espeak -f temp.txt");
    system(command);
#endif

    // Remove the temporary file
    remove("temp.txt");
}

int main() {
    // Buffer to hold user input
    char input[256];

    printf("Enter the text you want to convert to speech:\n");

    // Get user input
    if (fgets(input, sizeof(input), stdin) != NULL) {
        // Remove the newline character from the input
        input[strcspn(input, "\n")] = 0;
        // Call the text_to_speech function
        text_to_speech(input);
    } else {
        printf("Error reading input.\n");
    }

    return 0;
}
