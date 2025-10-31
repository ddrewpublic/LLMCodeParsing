// File: dice_roll_simulation.c

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_DICE 2
#define NUM_SIDES 6
#define NUM_ROLLS 1000000

// Function to roll the dice
int roll_dice(int num_dice, int num_sides) {
    int total = 0;
    for (int i = 0; i < num_dice; i++) {
        total += (rand() % num_sides) + 1; // Roll the dice (1 to num_sides)
    }
    return total;
}

// Function to visualize the results
void visualize_results(int *results, int num_rolls) {
    printf("\nRoll Results Visualization:\n");
    for (int i = NUM_DICE; i <= NUM_DICE * NUM_SIDES; i++) {
        if (results[i] > 0) {
            printf("%2d: ", i);
            int bar_length = (results[i] * 50) / num_rolls; // Scale the bar length
            for (int j = 0; j < bar_length; j++) {
                printf("#"); // Print the bar
            }
            printf(" (%d)\n", results[i]); // Print the count
        }
    }
}

int main() {
    // Seed the random number generator
    srand(time(NULL));

    // Array to store the count of each possible roll result
    int results[NUM_DICE * NUM_SIDES + 1] = {0};

    // Simulate rolling the dice
    for (int i = 0; i < NUM_ROLLS; i++) {
        int roll_result = roll_dice(NUM_DICE, NUM_SIDES);
        results[roll_result]++; // Increment the count for this result
    }

    // Print the results
    printf("Results of rolling %d %d-sided dice %d times:\n", NUM_DICE, NUM_SIDES, NUM_ROLLS);
    for (int i = NUM_DICE; i <= NUM_DICE * NUM_SIDES; i++) {
        printf("Roll %2d: %d times (%.2f%%)\n", i, results[i], (results[i] / (float)NUM_ROLLS) * 100);
    }

    // Visualize the results
    visualize_results(results, NUM_ROLLS);

    return 0;
}
