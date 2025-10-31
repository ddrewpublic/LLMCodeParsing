// File: bulk_rename.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_FILENAME_LENGTH 256
#define MAX_PATH_LENGTH 512

// Function to rename files based on a pattern
void rename_files(const char *directory, const char *old_pattern, const char *new_pattern) {
    struct dirent *entry;
    DIR *dp = opendir(directory);
    
    if (dp == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dp)) != NULL) {
        // Skip current and parent directories
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Check if the filename contains the old pattern
        if (strstr(entry->d_name, old_pattern) != NULL) {
            char old_file_path[MAX_PATH_LENGTH];
            char new_file_path[MAX_PATH_LENGTH];

            // Create full paths for old and new filenames
            snprintf(old_file_path, sizeof(old_file_path), "%s/%s", directory, entry->d_name);
            snprintf(new_file_path, sizeof(new_file_path), "%s/%s", directory, 
                     new_pattern);

            // Replace the old pattern in the new filename with the original filename
            char *pos = strstr(entry->d_name, old_pattern);
            if (pos) {
                // Calculate new filename with replaced pattern
                size_t prefix_length = pos - entry->d_name;
                snprintf(new_file_path, sizeof(new_file_path), "%s/%.*s%s", directory,
                         (int)prefix_length, entry->d_name, pos + strlen(old_pattern));
            }

            // Rename the file
            if (rename(old_file_path, new_file_path) != 0) {
                perror("rename");
            } else {
                printf("Renamed: %s -> %s\n", old_file_path, new_file_path);
            }
        }
    }

    closedir(dp);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <directory> <old_pattern> <new_pattern>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *directory = argv[1];
    const char *old_pattern = argv[2];
    const char *new_pattern = argv[3];

    rename_files(directory, old_pattern, new_pattern);

    return EXIT_SUCCESS;
}
