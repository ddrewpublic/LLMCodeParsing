// File: ascii_bar_chart.c
#include <stdio.h>
#include <stdlib.h>

#define MAX_BARS 100
#define MAX_HEIGHT 20

// Function to print a single bar
void print_bar(int height, char label) {
    printf("%c: ", label);
    for (int i = 0; i < height; i++) {
        putchar('#');
    }
    putchar('\n');
}

// Function to create an ASCII bar chart from an array of values
void create_bar_chart(int values[], int size) {
    int max_value = 0;

    // Find the maximum value to scale the bars
    for (int i = 0; i < size; i++) {
        if (values[i] > max_value) {
            max_value = values[i];
        }
    }

    // Print the bars scaled to a maximum height
    for (int i = 0; i < size; i++) {
        int height = (values[i] * MAX_HEIGHT) / max_value;
        print_bar(height, 'A' + i); // Use letters A, B, C, etc. for labels
    }
}

// Function to read numbers from user input
int read_numbers(int values[], int max_size) {
    int count = 0;
    printf("Enter up to %d numbers (negative number to stop):\n", max_size);
    while (count < max_size) {
        int num;
        printf("Number %d: ", count + 1);
        scanf("%d", &num);
        if (num < 0) {
            break; // Stop on negative number
        }
        values[count++] = num;
    }
    return count; // Return the number of valid entries
}

int main() {
    int values[MAX_BARS];
    int count;

    // Read numbers from user
    count = read_numbers(values, MAX_BARS);

    // Create and display the bar chart
    if (count > 0) {
        create_bar_chart(values, count);
    } else {
        printf("No valid numbers entered.\n");
    }

    return 0;
}
