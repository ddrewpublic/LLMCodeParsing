// File: dice_coin_roller.c

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function to roll a virtual dice
int roll_dice() {
    return (rand() % 6) + 1; // Returns a number between 1 and 6
}

// Function to flip a coin
const char* flip_coin() {
    return (rand() % 2) ? "Heads" : "Tails"; // Returns "Heads" or "Tails"
}

// Function to display the menu
void display_menu() {
    printf("Welcome to the Virtual Dice Roller and Coin Flipper!\n");
    printf("Please choose an option:\n");
    printf("1. Roll a Dice\n");
    printf("2. Flip a Coin\n");
    printf("3. Exit\n");
}

// Main function
int main() {
    int choice;
    
    // Seed the random number generator
    srand(time(NULL));

    do {
        display_menu(); // Show the menu
        printf("Enter your choice: ");
        scanf("%d", &choice); // Get user choice
        
        switch (choice) {
            case 1: {
                int result = roll_dice(); // Roll the dice
                printf("You rolled a %d!\n\n", result);
                break;
            }
            case 2: {
                const char* result = flip_coin(); // Flip the coin
                printf("You flipped: %s!\n\n", result);
                break;
            }
            case 3: {
                printf("Exiting the program. Goodbye!\n");
                break;
            }
            default: {
                printf("Invalid choice, please try again.\n\n");
                break;
            }
        }
    } while (choice != 3); // Continue until user chooses to exit

    return 0; // End of the program
}
