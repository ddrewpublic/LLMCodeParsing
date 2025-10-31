// File: birthday_email_sender.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_RECIPIENTS 100
#define EMAIL_LENGTH 256
#define NAME_LENGTH 100
#define DATE_LENGTH 11

typedef struct {
    char name[NAME_LENGTH];
    char email[EMAIL_LENGTH];
    char birthday[DATE_LENGTH]; // Format: YYYY-MM-DD
} Recipient;

// Function to load recipients from a file
int load_recipients(const char *filename, Recipient recipients[]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open recipients file");
        return -1;
    }

    int count = 0;
    while (fscanf(file, "%99[^,],%255[^,],%10s\n", recipients[count].name, recipients[count].email, recipients[count].birthday) == 3) {
        count++;
        if (count >= MAX_RECIPIENTS) break;
    }

    fclose(file);
    return count;
}

// Function to send an email (dummy implementation)
void send_email(const char *to, const char *name) {
    // In a real application, this function would interface with an email server
    printf("Sending birthday email to %s at %s\n", name, to);
}

// Function to check if today is the birthday of the recipient
int is_birthday_today(const char *birthday) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    char today[DATE_LENGTH];
    snprintf(today, DATE_LENGTH, "%04d-%02d-%02d", tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday);

    return strcmp(birthday, today) == 0;
}

// Main function
int main() {
    Recipient recipients[MAX_RECIPIENTS];
    int count = load_recipients("recipients.txt", recipients);

    if (count < 0) {
        return EXIT_FAILURE; // Exit if loading failed
    }

    // Iterate through all recipients and send emails if today is their birthday
    for (int i = 0; i < count; i++) {
        if (is_birthday_today(recipients[i].birthday)) {
            send_email(recipients[i].email, recipients[i].name);
        }
    }

    return EXIT_SUCCESS;
}
