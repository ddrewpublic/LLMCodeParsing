// File: organize_files.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define MAX_PATH 1024
#define MAX_TYPE 64
#define DATE_STR_LEN 20

// Function to get the file type based on its extension
void get_file_type(const char *filename, char *file_type) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) {
        strcpy(file_type, "unknown");
    } else {
        strcpy(file_type, dot + 1);
    }
}

// Function to create a directory if it does not exist
void create_directory(const char *dir_name) {
    struct stat st = {0};
    if (stat(dir_name, &st) == -1) {
        mkdir(dir_name, 0700);
    }
}

// Function to format the date string from a timestamp
void format_date(time_t timestamp, char *date_str) {
    struct tm *tm_info = localtime(&timestamp);
    strftime(date_str, DATE_STR_LEN, "%Y-%m-%d", tm_info);
}

// Function to organize files in the specified directory
void organize_files(const char *directory) {
    DIR *dir;
    struct dirent *entry;
    char file_path[MAX_PATH], file_type[MAX_TYPE], date_str[DATE_STR_LEN];
    struct stat file_stat;

    // Open the directory
    dir = opendir(directory);
    if (!dir) {
        perror("Failed to open directory");
        return;
    }

    // Iterate through each file in the directory
    while ((entry = readdir(dir)) != NULL) {
        // Skip the current and parent directory entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construct the full file path
        snprintf(file_path, sizeof(file_path), "%s/%s", directory, entry->d_name);
        
        // Get file stats
        if (stat(file_path, &file_stat) == -1) {
            perror("Failed to get file stats");
            continue;
        }

        // Get the file type
        get_file_type(entry->d_name, file_type);
        
        // Format the date
        format_date(file_stat.st_mtime, date_str);

        // Create the directory for the file type and date
        char new_dir[MAX_PATH];
        snprintf(new_dir, sizeof(new_dir), "%s/%s/%s", directory, file_type, date_str);
        create_directory(new_dir);

        // Move the file to the new directory
        char new_file_path[MAX_PATH];
        snprintf(new_file_path, sizeof(new_file_path), "%s/%s/%s/%s", directory, file_type, date_str, entry->d_name);
        if (rename(file_path, new_file_path) == -1) {
            perror("Failed to move file");
        }
    }

    // Close the directory
    closedir(dir);
}

int main(int argc, char *argv[]) {
    // Check for valid arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Organize files in the specified directory
    organize_files(argv[1]);

    return EXIT_SUCCESS;
}
