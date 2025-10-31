// File: daily_weather_email.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Function to get the current date as a string
void get_current_date(char *date_str, size_t size) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(date_str, size, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

// Function to compose the email content
void compose_email(const char *weather_update, const char *date, char *email_content, size_t size) {
    snprintf(email_content, size, "Subject: Daily Weather Update for %s\n\n%s\n", date, weather_update);
}

// Function to send the email using the system's mail command
int send_email(const char *email_content) {
    // Create a temporary file to hold the email content
    FILE *fp = fopen("email.txt", "w");
    if (fp == NULL) {
        perror("Failed to create email file");
        return -1;
    }
    fprintf(fp, "%s", email_content);
    fclose(fp);

    // Send the email using the mail command
    // Note: This requires a mail transfer agent (MTA) to be configured on the system
    char command[512];
    snprintf(command, sizeof(command), "sendmail recipient@example.com < email.txt");
    int result = system(command);

    // Clean up the temporary file
    remove("email.txt");
    return result;
}

// Function to get weather update (simulated for this example)
void get_weather_update(char *weather_update, size_t size) {
    // In a real application, you would fetch this from a weather API
    snprintf(weather_update, size, "Today's weather is sunny with a high of 25°C and a low of 15°C.");
}

int main() {
    char date[11]; // YYYY-MM-DD format
    char weather_update[256];
    char email_content[512];

    // Get the current date
    get_current_date(date, sizeof(date));

    // Get the weather update
    get_weather_update(weather_update, sizeof(weather_update));

    // Compose the email
    compose_email(weather_update, date, email_content, sizeof(email_content));

    // Send the email
    if (send_email(email_content) == 0) {
        printf("Weather update email sent successfully!\n");
    } else {
        printf("Failed to send email.\n");
    }

    return 0;
}
