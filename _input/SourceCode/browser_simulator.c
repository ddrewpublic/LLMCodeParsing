// File: browser_simulator.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LENGTH 100
#define MAX_FORMS 10

// Structure to hold form data
typedef struct {
    char name[MAX_INPUT_LENGTH];
    char email[MAX_INPUT_LENGTH];
    char message[MAX_INPUT_LENGTH];
} FormData;

// Function to fill the form
void fillForm(FormData *form) {
    printf("Filling the form...\n");

    printf("Enter your name: ");
    fgets(form->name, MAX_INPUT_LENGTH, stdin);
    form->name[strcspn(form->name, "\n")] = 0; // Remove newline

    printf("Enter your email: ");
    fgets(form->email, MAX_INPUT_LENGTH, stdin);
    form->email[strcspn(form->email, "\n")] = 0; // Remove newline

    printf("Enter your message: ");
    fgets(form->message, MAX_INPUT_LENGTH, stdin);
    form->message[strcspn(form->message, "\n")] = 0; // Remove newline
}

// Function to simulate form submission
void submitForm(const FormData *form) {
    printf("\nSubmitting the form...\n");
    printf("Name: %s\n", form->name);
    printf("Email: %s\n", form->email);
    printf("Message: %s\n", form->message);
    printf("Form submitted successfully!\n");
}

// Function to simulate a browser action
void simulateBrowserAction() {
    FormData form;
    fillForm(&form);
    submitForm(&form);
}

int main() {
    int choice;

    printf("Welcome to the Browser Simulator!\n");
    do {
        printf("\nMenu:\n");
        printf("1. Fill and Submit a Form\n");
        printf("2. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);
        getchar(); // Clear newline from buffer

        switch (choice) {
            case 1:
                simulateBrowserAction();
                break;
            case 2:
                printf("Exiting the simulator. Goodbye!\n");
                break;
            default:
                printf("Invalid option. Please try again.\n");
        }
    } while (choice != 2);

    return 0;
}
