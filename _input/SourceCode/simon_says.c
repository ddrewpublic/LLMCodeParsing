// File: simon_says.c

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>

#define MAX_SEQUENCE_LENGTH 100
#define NUM_COLORS 4

// Function prototypes
void display_sequence(int sequence[], int length);
void get_user_input(int user_sequence[], int length);
int check_sequence(int sequence[], int user_sequence[], int length);
void clear_screen();
char getch();

int main() {
    srand(time(NULL));
    int sequence[MAX_SEQUENCE_LENGTH];
    int user_sequence[MAX_SEQUENCE_LENGTH];
    int round = 1;
    int game_over = 0;

    while (!game_over) {
        sequence[round - 1] = rand() % NUM_COLORS;
        printf("Round %d\n", round);
        display_sequence(sequence, round);
        get_user_input(user_sequence, round);

        if (check_sequence(sequence, user_sequence, round) == 0) {
            printf("❌ Game Over! You reached round %d.\n", round);
            game_over = 1;
        } else {
            printf("✅ Correct! Moving to round %d.\n\n", round + 1);
            usleep(1000000); // Pause briefly
            clear_screen();
            round++;
        }
    }

    return 0;
}

// Show the sequence
void display_sequence(int sequence[], int length) {
    for (int i = 0; i < length; i++) {
        printf("Color %d: ", sequence[i]);
        switch (sequence[i]) {
            case 0: printf("🔴 Red\n"); break;
            case 1: printf("🟢 Green\n"); break;
            case 2: printf("🔵 Blue\n"); break;
            case 3: printf("🟡 Yellow\n"); break;
        }
        fflush(stdout);
        usleep(1000000);
        clear_screen();
    }
}

// Read user's input
void get_user_input(int user_sequence[], int length) {
    printf("🎮 Enter the sequence (0-3) for round %d:\n", length);
    for (int i = 0; i < length; i++) {
        char ch;
        do {
            ch = getch();
        } while (ch < '0' || ch > '3');

        user_sequence[i] = ch - '0';
        printf("  You entered: %d\n", user_sequence[i]);
        usleep(300000);
    }
}

// Sequence check
int check_sequence(int sequence[], int user_sequence[], int length) {
    for (int i = 0; i < length; i++) {
        if (sequence[i] != user_sequence[i]) {
            return 0;
        }
    }
    return 1;
}

// Clear screen (Unix)
void clear_screen() {
    system("clear");
}

// Cross-platform getch() replacement (Unix version)
char getch() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);            // Save old settings
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);          // Disable buffering and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);   // Apply new settings
    read(STDIN_FILENO, &ch, 1);                // Read a single char
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);   // Restore old settings
    return ch;
}
