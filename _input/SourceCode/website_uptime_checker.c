// File: website_uptime_checker.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_URL_LENGTH 256
#define MAX_COMMAND_LENGTH 512
#define CHECK_INTERVAL 5 // seconds

// Function to check website uptime and response speed
void check_website(const char *url) {
    char command[MAX_COMMAND_LENGTH];
    snprintf(command, sizeof(command), "curl -o /dev/null -s -w \"%%{http_code} %%{time_total}\\n\" %s", url);

    // Execute the command and open a pipe to read the output
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        perror("Failed to run command");
        return;
    }

    // Read the output from curl
    char buffer[128];
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        int http_code;
        double response_time;
        // Parse the HTTP code and response time
        sscanf(buffer, "%d %lf", &http_code, &response_time);
        
        // Check the HTTP response code
        if (http_code == 200) {
            printf("Website is UP. Response time: %.3f seconds\n", response_time);
        } else {
            printf("Website is DOWN. HTTP code: %d\n", http_code);
        }
    } else {
        printf("Failed to read output from command.\n");
    }

    // Close the pipe
    pclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <URL>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *url = argv[1];

    while (1) {
        check_website(url);
        sleep(CHECK_INTERVAL); // Wait for the specified interval before the next check
    }

    return EXIT_SUCCESS;
}
