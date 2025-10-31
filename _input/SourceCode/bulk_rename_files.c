// File: bulk_rename_files.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_FILENAME_LENGTH 256
#define MAX_PATH_LENGTH 512

// Function to generate new filename based on a pattern
void generate_new_filename(const char *old_name, const char *pattern, char *new_name) {
    // Example pattern: "newfile_%d.txt" where %d will be replaced with a number
    static int counter = 1;
    snprintf(new_name, MAX_FILENAME_LENGTH, pattern, counter++);
}

// Function to rename files in the specified directory based on a pattern
void rename_files(const char *directory, const char *pattern) {
    struct dirent *entry;
    DIR *dp = opendir(directory);
    char old_path[MAX_PATH_LENGTH];
    char new_path[MAX_PATH_LENGTH];
    
    if (dp == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dp)) != NULL) {
        // Skip the current and parent directory entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construct old file path
        snprintf(old_path, sizeof(old_path), "%s/%s", directory, entry->d_name);

        // Generate new filename
        generate_new_filename(entry->d_name, pattern, new_path);
        snprintf(new_path, sizeof(new_path), "%s/%s", directory, new_path);

        // Rename the file
        if (rename(old_path, new_path) != 0) {
            perror("rename");
        } else {
            printf("Renamed: %s -> %s\n", old_path, new_path);
        }
    }

    closedir(dp);
}

int main(int argc, char *argv[]) {
    // Check for correct number of arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <directory> <pattern>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *directory = argv[1];
    const char *pattern = argv[2];

    // Rename files in the specified directory based on the provided pattern
    rename_files(directory, pattern);

    return EXIT_SUCCESS;
}
