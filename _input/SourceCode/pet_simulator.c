// File: pet_simulator.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PETS 10
#define NAME_LENGTH 20

typedef struct {
    char name[NAME_LENGTH];
    int hunger; // 0 to 100
    int happiness; // 0 to 100
    int age; // in years
} Pet;

void initialize_pet(Pet *pet, const char *name) {
    strncpy(pet->name, name, NAME_LENGTH);
    pet->hunger = 50; // starting hunger level
    pet->happiness = 50; // starting happiness level
    pet->age = 0; // starting age
}

void feed_pet(Pet *pet) {
    pet->hunger -= 10;
    if (pet->hunger < 0) pet->hunger = 0;
    pet->happiness += 5;
    if (pet->happiness > 100) pet->happiness = 100;
    printf("%s has been fed!\n", pet->name);
}

void play_with_pet(Pet *pet) {
    pet->happiness += 10;
    if (pet->happiness > 100) pet->happiness = 100;
    pet->hunger += 5;
    if (pet->hunger > 100) pet->hunger = 100;
    printf("You played with %s!\n", pet->name);
}

void age_pet(Pet *pet) {
    pet->age++;
    pet->happiness -= 5;
    if (pet->happiness < 0) pet->happiness = 0;
}

void display_pet_status(const Pet *pet) {
    printf("\n--- %s's Status ---\n", pet->name);
    printf("Age: %d years\n", pet->age);
    printf("Hunger: %d/100\n", pet->hunger);
    printf("Happiness: %d/100\n", pet->happiness);
    printf("--------------------\n");
}

void simulate_day(Pet *pet) {
    age_pet(pet);
    pet->hunger += 5;
    if (pet->hunger > 100) pet->hunger = 100;
}

void display_menu() {
    printf("\nPet Simulator Menu:\n");
    printf("1. Feed Pet\n");
    printf("2. Play with Pet\n");
    printf("3. Check Pet Status\n");
    printf("4. Exit\n");
    printf("Choose an option: ");
}

int main() {
    Pet pets[MAX_PETS];
    int pet_count = 0;
    int choice;
    char name[NAME_LENGTH];

    srand(time(NULL)); // Seed random number generator

    // Initialize pets
    while (pet_count < MAX_PETS) {
        printf("Enter the name of pet %d (or type 'exit' to finish): ", pet_count + 1);
        scanf("%s", name);
        if (strcmp(name, "exit") == 0) break;
        initialize_pet(&pets[pet_count], name);
        pet_count++;
    }

    // Main simulation loop
    while (1) {
        for (int i = 0; i < pet_count; i++) {
            simulate_day(&pets[i]);
        }

        display_menu();
        scanf("%d", &choice);

        if (choice == 1) {
            for (int i = 0; i < pet_count; i++) {
                feed_pet(&pets[i]);
            }
        } else if (choice == 2) {
            for (int i = 0; i < pet_count; i++) {
                play_with_pet(&pets[i]);
            }
        } else if (choice == 3) {
            for (int i = 0; i < pet_count; i++) {
                display_pet_status(&pets[i]);
            }
        } else if (choice == 4) {
            printf("Exiting the Pet Simulator. Goodbye!\n");
            break;
        } else {
            printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
