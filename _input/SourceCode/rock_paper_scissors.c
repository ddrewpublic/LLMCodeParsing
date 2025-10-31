// File: rock_paper_scissors.c

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function to get the computer's choice
int get_computer_choice() {
    return rand() % 3; // 0: Rock, 1: Paper, 2: Scissors
}

// Function to determine the winner
const char* determine_winner(int user_choice, int computer_choice) {
    if (user_choice == computer_choice) {
        return "It's a tie!";
    } else if ((user_choice == 0 && computer_choice == 2) || 
               (user_choice == 1 && computer_choice == 0) || 
               (user_choice == 2 && computer_choice == 1)) {
        return "You win!";
    } else {
        return "Computer wins!";
    }
}

// Function to display choices
void display_choices(int user_choice, int computer_choice) {
    const char* choices[] = {"Rock", "Paper", "Scissors"};
    printf("You chose: %s\n", choices[user_choice]);
    printf("Computer chose: %s\n", choices[computer_choice]);
}

// Main function to run the game
int main() {
    srand(time(NULL)); // Seed the random number generator
    int user_choice;
    char play_again;

    printf("Welcome to Rock-Paper-Scissors!\n");

    do {
        printf("Enter your choice (0: Rock, 1: Paper, 2: Scissors): ");
        scanf("%d", &user_choice);

        // Validate user input
        while (user_choice < 0 || user_choice > 2) {
            printf("Invalid choice. Please enter 0, 1, or 2: ");
            scanf("%d", &user_choice);
        }

        int computer_choice = get_computer_choice(); // Get computer's choice
        display_choices(user_choice, computer_choice); // Show choices
        printf("%s\n", determine_winner(user_choice, computer_choice)); // Determine winner

        printf("Do you want to play again? (y/n): ");
        scanf(" %c", &play_again); // Space before %c to consume newline

    } while (play_again == 'y' || play_again == 'Y');

    printf("Thank you for playing!\n");
    return 0;
}
