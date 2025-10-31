// File: cleanup_temp_duplicate_files.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_PATH 1024
#define TEMP_FILE_PREFIX "temp_"
#define MAX_DUPLICATES 100

// Function to check if a file is temporary
int is_temp_file(const char *filename) {
    return strncmp(filename, TEMP_FILE_PREFIX, strlen(TEMP_FILE_PREFIX)) == 0;
}

// Function to compare two files for duplication
int are_files_duplicate(const char *file1, const char *file2) {
    struct stat stat1, stat2;
    if (stat(file1, &stat1) != 0 || stat(file2, &stat2) != 0) {
        return 0; // One of the files does not exist
    }

    // Check if the sizes are equal
    if (stat1.st_size != stat2.st_size) {
        return 0;
    }

    // Compare contents
    FILE *f1 = fopen(file1, "rb");
    FILE *f2 = fopen(file2, "rb");
    if (!f1 || !f2) {
        if (f1) fclose(f1);
        if (f2) fclose(f2);
        return 0; // Failed to open one of the files
    }

    char buffer1[4096], buffer2[4096];
    size_t bytesRead1, bytesRead2;

    do {
        bytesRead1 = fread(buffer1, 1, sizeof(buffer1), f1);
        bytesRead2 = fread(buffer2, 1, sizeof(buffer2), f2);

        if (bytesRead1 != bytesRead2 || memcmp(buffer1, buffer2, bytesRead1) != 0) {
            fclose(f1);
            fclose(f2);
            return 0; // Files differ
        }
    } while (bytesRead1 > 0 && bytesRead2 > 0);

    fclose(f1);
    fclose(f2);

    return 1; // Files are identical
}

// Function to remove temporary files and duplicates
void cleanup_files(const char *directory) {
    DIR *dir;
    struct dirent *entry;
    char filepath[MAX_PATH];
    char *files[MAX_DUPLICATES];
    int file_count = 0;

    if ((dir = opendir(directory)) == NULL) {
        perror("opendir");
        return;
    }

    // Traverse the directory
    while ((entry = readdir(dir)) != NULL) {
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(filepath, sizeof(filepath), "%s/%s", directory, entry->d_name);

        // Skip non-regular files
        struct stat path_stat;
        if (stat(filepath, &path_stat) != 0 || !S_ISREG(path_stat.st_mode)) {
            continue;
        }

        // Check if the file is temporary
        if (is_temp_file(entry->d_name)) {
            printf("Removing temporary file: %s\n", filepath);
            remove(filepath);
            continue;
        }

        // Check for duplicates
        int is_duplicate = 0;
        for (int i = 0; i < file_count; i++) {
            if (are_files_duplicate(filepath, files[i])) {
                printf("Removing duplicate file: %s (duplicate of %s)\n", filepath, files[i]);
                remove(filepath);
                is_duplicate = 1;
                break;
            }
        }

        // If not a duplicate, store the file for future comparison
        if (!is_duplicate && file_count < MAX_DUPLICATES) {
            files[file_count++] = strdup(filepath);
        }
    }

    closedir(dir);

    // Free allocated memory
    for (int i = 0; i < file_count; i++) {
        free(files[i]);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory_path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    cleanup_files(argv[1]);
    return EXIT_SUCCESS;
}
