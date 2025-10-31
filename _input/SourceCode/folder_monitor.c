// File: folder_monitor.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

#define MAX_FILES 1024
#define FOLDER_PATH "./watched_folder"

// Function to get the current list of files in the directory
void get_file_list(const char *path, char file_list[MAX_FILES][256], int *file_count) {
    struct dirent *entry;
    DIR *dp = opendir(path);
    *file_count = 0;

    if (dp == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_type == DT_REG) { // Only regular files
            snprintf(file_list[*file_count], 256, "%s/%s", path, entry->d_name);
            (*file_count)++;
        }
    }
    closedir(dp);
}

// Function to check if a file exists in the list
int file_exists(const char *filename, char file_list[MAX_FILES][256], int file_count) {
    for (int i = 0; i < file_count; i++) {
        if (strcmp(filename, file_list[i]) == 0) {
            return 1; // File exists
        }
    }
    return 0; // File does not exist
}

// Main function to monitor the folder
int main() {
    char file_list[MAX_FILES][256];
    int file_count = 0;
    int previous_count = 0;

    // Initial load of files
    get_file_list(FOLDER_PATH, file_list, &file_count);
    printf("Monitoring folder: %s\n", FOLDER_PATH);

    while (1) {
        sleep(2); // Check every 2 seconds
        get_file_list(FOLDER_PATH, file_list, &file_count);

        // Check for new files
        if (file_count > previous_count) {
            for (int i = 0; i < file_count; i++) {
                if (!file_exists(file_list[i], file_list, previous_count)) {
                    printf("New file added: %s\n", file_list[i]);
                }
            }
        }
        previous_count = file_count; // Update the previous count
    }

    return 0;
}
