// File: app_time_tracker.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_APPS 100
#define APP_NAME_LENGTH 50

typedef struct {
    char name[APP_NAME_LENGTH];
    time_t total_time; // Total time spent on the application
    time_t start_time; // Time when tracking started
    int is_tracking; // Flag to indicate if tracking is active
} App;

App apps[MAX_APPS];
int app_count = 0;

// Function to find an application by name
int find_app(const char *name) {
    for (int i = 0; i < app_count; i++) {
        if (strcmp(apps[i].name, name) == 0) {
            return i; // Return index if found
        }
    }
    return -1; // Not found
}

// Function to start tracking time for an application
void start_tracking(const char *name) {
    int index = find_app(name);

    if (index == -1) {
        // If app is not found, add a new one
        if (app_count < MAX_APPS) {
            strncpy(apps[app_count].name, name, APP_NAME_LENGTH);
            apps[app_count].total_time = 0;
            apps[app_count].is_tracking = 1;
            apps[app_count].start_time = time(NULL);
            app_count++;
            printf("Started tracking time for: %s\n", name);
        } else {
            printf("Maximum number of applications reached.\n");
        }
    } else {
        // If app is found, resume tracking
        if (!apps[index].is_tracking) {
            apps[index].start_time = time(NULL);
            apps[index].is_tracking = 1;
            printf("Resumed tracking time for: %s\n", name);
        } else {
            printf("Already tracking: %s\n", name);
        }
    }
}

// Function to stop tracking time for an application
void stop_tracking(const char *name) {
    int index = find_app(name);
    
    if (index != -1 && apps[index].is_tracking) {
        time_t end_time = time(NULL);
        apps[index].total_time += (end_time - apps[index].start_time);
        apps[index].is_tracking = 0;
        printf("Stopped tracking time for: %s\n", name);
    } else {
        printf("No active tracking found for: %s\n", name);
    }
}

// Function to display the total time spent on each application
void display_time() {
    printf("\nApplication Time Tracker:\n");
    printf("-------------------------------------------------\n");
    for (int i = 0; i < app_count; i++) {
        printf("Application: %s | Total Time: %ld seconds\n", apps[i].name, apps[i].total_time);
    }
    printf("-------------------------------------------------\n");
}

// Main function to run the application
int main() {
    char command[APP_NAME_LENGTH + 10]; // Buffer for user input
    char app_name[APP_NAME_LENGTH]; // Buffer for application name

    printf("Welcome to the Application Time Tracker!\n");
    printf("Commands: start <app_name>, stop <app_name>, display, exit\n");

    while (1) {
        printf("\nEnter command: ");
        fgets(command, sizeof(command), stdin); // Get user input
        sscanf(command, "%s %49[^\n]", command, app_name); // Parse command and app name

        if (strcmp(command, "start") == 0) {
            start_tracking(app_name);
        } else if (strcmp(command, "stop") == 0) {
            stop_tracking(app_name);
        } else if (strcmp(command, "display") == 0) {
            display_time();
        } else if (strcmp(command, "exit") == 0) {
            printf("Exiting Application Time Tracker. Goodbye!\n");
            break;
        } else {
            printf("Unknown command. Please try again.\n");
        }
    }

    return 0;
}
