// File: count_lines_of_code.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_PATH 1024

// Function to count lines in a single file
int count_lines_in_file(const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Could not open file");
        return 0;
    }

    int lines = 0;
    char buffer[1024];

    // Read lines until end of file
    while (fgets(buffer, sizeof(buffer), file)) {
        lines++;
    }

    fclose(file);
    return lines;
}

// Function to check if a file has a valid extension
int is_source_file(const char *file_name) {
    const char *ext = strrchr(file_name, '.');
    return ext && (strcmp(ext, ".c") == 0 || strcmp(ext, ".h") == 0 || strcmp(ext, ".cpp") == 0);
}

// Recursive function to traverse directories and count lines of code
void count_lines_in_directory(const char *dir_path, int *total_lines) {
    struct dirent *entry;
    struct stat file_stat;
    char path[MAX_PATH];

    DIR *dir = opendir(dir_path);
    if (!dir) {
        perror("Could not open directory");
        return;
    }

    // Iterate over each entry in the directory
    while ((entry = readdir(dir)) != NULL) {
        // Skip the current and parent directory entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construct the full path
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);
        if (stat(path, &file_stat) == -1) {
            perror("Could not get file status");
            continue;
        }

        // If it's a directory, recurse into it
        if (S_ISDIR(file_stat.st_mode)) {
            count_lines_in_directory(path, total_lines);
        } else if (S_ISREG(file_stat.st_mode) && is_source_file(entry->d_name)) {
            // If it's a source file, count its lines
            *total_lines += count_lines_in_file(path);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int total_lines = 0;
    count_lines_in_directory(argv[1], &total_lines);

    printf("Total lines of code in directory '%s': %d\n", argv[1], total_lines);
    return EXIT_SUCCESS;
}
