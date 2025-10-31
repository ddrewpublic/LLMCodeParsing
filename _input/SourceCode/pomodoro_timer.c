// File: pomodoro_timer.c

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define WORK_DURATION 25 // Work duration in minutes
#define BREAK_DURATION 5  // Break duration in minutes
#define LONG_BREAK_DURATION 15 // Long break duration in minutes
#define LONG_BREAK_INTERVAL 4 // After how many work sessions to take a long break

void countdown(int seconds) {
    while (seconds > 0) {
        int minutes = seconds / 60;
        int secs = seconds % 60;
        printf("\rTime left: %02d:%02d", minutes, secs);
        fflush(stdout);
        sleep(1);
        seconds--;
    }
    printf("\n");
}

void start_pomodoro() {
    int work_sessions = 0;
    char choice;

    while (1) {
        // Start work session
        printf("Starting work session for %d minutes...\n", WORK_DURATION);
        countdown(WORK_DURATION * 60);
        work_sessions++;

        // Check if it's time for a long break
        if (work_sessions % LONG_BREAK_INTERVAL == 0) {
            printf("Time for a long break for %d minutes!\n", LONG_BREAK_DURATION);
            countdown(LONG_BREAK_DURATION * 60);
        } else {
            printf("Time for a short break for %d minutes!\n", BREAK_DURATION);
            countdown(BREAK_DURATION * 60);
        }

        // Ask if the user wants to continue
        printf("Do you want to continue? (y/n): ");
        scanf(" %c", &choice);
        if (choice != 'y' && choice != 'Y') {
            break;
        }
    }
    printf("Pomodoro session ended. Great job!\n");
}

int main() {
    printf("Welcome to the Pomodoro Timer!\n");
    start_pomodoro();
    return 0;
}
