// File: fantasy_character_generator.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Define maximum lengths for names
#define MAX_NAME_LENGTH 20
#define MAX_FIRST_NAMES 10
#define MAX_LAST_NAMES 10

// Function to generate a random name
void generate_random_name(char *name) {
    // Arrays of first names and last names
    const char *first_names[MAX_FIRST_NAMES] = {
        "Aeliana", "Brom", "Cindel", "Dorian", "Elysia",
        "Fenwick", "Gwendolyn", "Haldor", "Isolde", "Jareth"
    };
    
    const char *last_names[MAX_LAST_NAMES] = {
        "Stormrider", "Moonshadow", "Fireheart", "Dewwhisper", "Ironfist",
        "Wolfsbane", "Nightshade", "Silverleaf", "Brightstar", "Darkwater"
    };

    // Seed the random number generator
    int first_index = rand() % MAX_FIRST_NAMES;
    int last_index = rand() % MAX_LAST_NAMES;

    // Generate the name by combining a random first and last name
    snprintf(name, MAX_NAME_LENGTH, "%s %s", first_names[first_index], last_names[last_index]);
}

// Main function
int main() {
    // Initialize random number generator
    srand(time(NULL));

    // Number of names to generate
    int num_names;
    printf("Enter the number of random fantasy character names to generate: ");
    scanf("%d", &num_names);

    // Validate input
    if (num_names <= 0) {
        printf("Please enter a positive number.\n");
        return 1;
    }

    // Allocate memory for names
    char name[MAX_NAME_LENGTH];

    // Generate and print the names
    printf("\nGenerated Fantasy Character Names:\n");
    for (int i = 0; i < num_names; i++) {
        generate_random_name(name);
        printf("%d: %s\n", i + 1, name);
    }

    return 0;
}
