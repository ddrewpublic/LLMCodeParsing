// File: backup_to_zip.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define MAX_PATH 1024
#define BUFFER_SIZE 4096

// Function to create a zip file from a directory
void create_zip(const char *zip_filename, const char *directory) {
    char command[MAX_PATH + 50];
    snprintf(command, sizeof(command), "zip -r %s %s", zip_filename, directory);
    
    // Execute the command to create zip file
    int result = system(command);
    if (result == -1) {
        perror("Error creating zip file");
    }
}

// Function to check if the given path is a directory
int is_directory(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

// Function to backup folders
void backup_folders(const char *folders[], int folder_count, const char *backup_location) {
    for (int i = 0; i < folder_count; i++) {
        const char *folder = folders[i];
        if (is_directory(folder)) {
            char zip_filename[MAX_PATH];
            snprintf(zip_filename, sizeof(zip_filename), "%s/%s.zip", backup_location, folder);
            create_zip(zip_filename, folder);
        } else {
            fprintf(stderr, "Warning: %s is not a directory. Skipping...\n", folder);
        }
    }
}

int main(int argc, char *argv[]) {
    // Check for correct usage
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <backup_location> <folder1> <folder2> ... <folderN>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *backup_location = argv[1];
    const char **folders = (const char **)(argv + 2);
    int folder_count = argc - 2;

    // Create the backup location directory if it doesn't exist
    mkdir(backup_location, 0777);

    // Start the backup process
    backup_folders(folders, folder_count, backup_location);

    return EXIT_SUCCESS;
}
