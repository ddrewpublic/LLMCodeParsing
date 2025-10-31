// File: log_parser.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define MAX_LOG_ENTRIES 1000

typedef struct LogEntry {
    char timestamp[20];
    char logLevel[10];
    char message[MAX_LINE_LENGTH - 30];
} LogEntry;

void parseLogLine(const char *line, LogEntry *entry) {
    // Example log line format: "2023-10-01 12:00:00 INFO Log message here"
    sscanf(line, "%19s %19s %[^\n]", entry->timestamp, entry->logLevel, entry->message);
}

void printLogEntry(const LogEntry *entry) {
    // Print the log entry in a reformatted way
    printf("[%s] [%s]: %s\n", entry->timestamp, entry->logLevel, entry->message);
}

int main(int argc, char *argv[]) {
    // Check if a filename is provided
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <logfile>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    LogEntry entries[MAX_LOG_ENTRIES];
    int entryCount = 0;
    char line[MAX_LINE_LENGTH];

    // Read each line from the log file
    while (fgets(line, sizeof(line), file) && entryCount < MAX_LOG_ENTRIES) {
        // Parse the log line into a LogEntry structure
        parseLogLine(line, &entries[entryCount]);
        entryCount++;
    }

    fclose(file);

    // Print all parsed log entries
    for (int i = 0; i < entryCount; i++) {
        printLogEntry(&entries[i]);
    }

    return EXIT_SUCCESS;
}
