// File: extract_zip_recursively.c

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zip.h>

// Function to extract a zip file to a specified directory
int extract_zip(const char *zip_path, const char *extract_to) {
    int err = 0;
    zip_t *archive = zip_open(zip_path, 0, &err);
    if (!archive) {
        fprintf(stderr, "Error opening zip file %s: %d\n", zip_path, err);
        return -1;
    }

    // Create the extraction directory if it doesn't exist
    mkdir(extract_to, 0755);

    zip_int64_t num_entries = zip_get_num_entries(archive, 0);
    for (zip_int64_t i = 0; i < num_entries; i++) {
        const char *name = zip_get_name(archive, i, 0);
        if (!name) {
            fprintf(stderr, "Error getting name of file in zip: %s\n", zip_strerror(archive));
            zip_close(archive);
            return -1;
        }

        // Create full path for extraction
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", extract_to, name);

        if (name[strlen(name) - 1] == '/') {
            // It's a directory
            mkdir(full_path, 0755);
        } else {
            // It's a file
            struct zip_file *zfile = zip_fopen_index(archive, i, 0);
            if (!zfile) {
                fprintf(stderr, "Error opening file %s in zip: %s\n", name, zip_strerror(archive));
                zip_close(archive);
                return -1;
            }

            // Make sure parent directories exist
            char *last_slash = strrchr(full_path, '/');
            if (last_slash) {
                *last_slash = '\0';
                mkdir(full_path, 0755);
                *last_slash = '/';
            }

            FILE *out = fopen(full_path, "wb");
            if (!out) {
                fprintf(stderr, "Error creating file %s: %s\n", full_path, strerror(errno));
                zip_fclose(zfile);
                zip_close(archive);
                return -1;
            }

            char buffer[8192];
            zip_int64_t bytes_read;
            while ((bytes_read = zip_fread(zfile, buffer, sizeof(buffer))) > 0) {
                fwrite(buffer, 1, bytes_read, out);
            }

            fclose(out);
            zip_fclose(zfile);
        }
    }

    zip_close(archive);
    return 0;
}

// Function to recursively traverse directories and extract zip files
void traverse_and_extract(const char *base_path) {
    struct dirent *entry;
    DIR *dp = opendir(base_path);

    if (!dp) {
        fprintf(stderr, "Error opening directory %s: %s\n", base_path, strerror(errno));
        return;
    }

    while ((entry = readdir(dp))) {
        // Skip the current and parent directory entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construct the full path for the entry
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", base_path, entry->d_name);

        // Check if the entry is a directory
        struct stat statbuf;
        if (stat(path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
            traverse_and_extract(path);
        } else if (strstr(entry->d_name, ".zip") != NULL) {
            // Extract zip files
            printf("Extracting: %s\n", path);
            char extract_path[1024];
            snprintf(extract_path, sizeof(extract_path), "%s/extracted", base_path);
            extract_zip(path, extract_path);
        }
    }

    closedir(dp);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    traverse_and_extract(argv[1]);
    return EXIT_SUCCESS;
}
