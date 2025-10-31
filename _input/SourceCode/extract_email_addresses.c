// File: extract_email_addresses.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1024
#define MAX_EMAIL_LENGTH 256
#define MAX_EMAILS 100

// Function to check if a character is valid in an email address
int is_valid_email_char(char c) {
    return isalnum(c) || c == '.' || c == '_' || c == '-' || c == '@';
}

// Function to extract email addresses from a line of text
void extract_emails(const char *line, char emails[MAX_EMAILS][MAX_EMAIL_LENGTH], int *email_count) {
    const char *ptr = line;
    char email[MAX_EMAIL_LENGTH];
    int email_index = 0;
    int found_email = 0;

    while (*ptr) {
        // Reset email buffer
        email_index = 0;
        found_email = 0;

        // Look for the start of an email address
        while (*ptr && !isalnum(*ptr)) {
            ptr++;
        }

        // If we found a starting character, start extracting
        if (*ptr) {
            found_email = 1;
            while (*ptr && is_valid_email_char(*ptr) && email_index < MAX_EMAIL_LENGTH - 1) {
                email[email_index++] = *ptr;
                ptr++;
            }
            email[email_index] = '\0'; // Null terminate the email string

            // If we found a valid email, store it
            if (found_email && *email) {
                strcpy(emails[*email_count], email);
                (*email_count)++;
                if (*email_count >= MAX_EMAILS) {
                    break; // Limit reached
                }
            }
        }
    }
}

// Function to print the extracted email addresses
void print_emails(char emails[MAX_EMAILS][MAX_EMAIL_LENGTH], int email_count) {
    printf("Extracted Email Addresses:\n");
    for (int i = 0; i < email_count; i++) {
        printf("%s\n", emails[i]);
    }
}

int main() {
    char line[MAX_LINE_LENGTH];
    char emails[MAX_EMAILS][MAX_EMAIL_LENGTH];
    int email_count = 0;

    printf("Enter text (Ctrl+D to end input):\n");

    // Read lines from standard input until EOF
    while (fgets(line, sizeof(line), stdin)) {
        extract_emails(line, emails, &email_count);
    }

    // Print the extracted email addresses
    print_emails(emails, email_count);

    return 0;
}
