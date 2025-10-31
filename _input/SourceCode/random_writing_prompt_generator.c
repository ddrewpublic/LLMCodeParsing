// File: random_writing_prompt_generator.c

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_PROMPTS 100
#define MAX_LENGTH 256

// Function to load prompts from a file
int load_prompts(const char *filename, char prompts[MAX_PROMPTS][MAX_LENGTH]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Could not open prompts file");
        return -1;
    }

    int count = 0;
    while (count < MAX_PROMPTS && fgets(prompts[count], MAX_LENGTH, file)) {
        // Remove newline character from the end of the prompt
        prompts[count][strcspn(prompts[count], "\n")] = 0;
        count++;
    }

    fclose(file);
    return count;
}

// Function to generate a random writing prompt
void generate_random_prompt(char prompts[MAX_PROMPTS][MAX_LENGTH], int count) {
    if (count == 0) {
        printf("No prompts available.\n");
        return;
    }
    int random_index = rand() % count; // Get a random index
    printf("Your writing prompt: %s\n", prompts[random_index]);
}

int main() {
    char prompts[MAX_PROMPTS][MAX_LENGTH];
    int prompt_count;

    // Seed the random number generator
    srand((unsigned int)time(NULL));

    // Load prompts from a file
    prompt_count = load_prompts("prompts.txt", prompts);
    if (prompt_count < 0) {
        return 1; // Exit if there was an error loading prompts
    }

    // Generate and display a random writing prompt
    generate_random_prompt(prompts, prompt_count);

    return 0;
}
