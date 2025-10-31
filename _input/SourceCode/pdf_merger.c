// File: pdf_merger.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to read a file into memory
char *read_file(const char *filename, long *file_size) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(*file_size);
    if (!buffer) {
        perror("Failed to allocate memory");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, *file_size, file);
    fclose(file);
    return buffer;
}

// Function to write a buffer to a file
int write_file(const char *filename, const char *buffer, long size) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to create file");
        return -1;
    }

    fwrite(buffer, 1, size, file);
    fclose(file);
    return 0;
}

// Function to merge PDF files
int merge_pdfs(int pdf_count, char **pdf_files, const char *output_file) {
    long total_size = 0;
    char **buffers = malloc(pdf_count * sizeof(char *));
    long *sizes = malloc(pdf_count * sizeof(long));

    // Read all PDF files into memory
    for (int i = 0; i < pdf_count; i++) {
        buffers[i] = read_file(pdf_files[i], &sizes[i]);
        if (!buffers[i]) {
            // Clean up on failure
            for (int j = 0; j < i; j++) {
                free(buffers[j]);
            }
            free(buffers);
            free(sizes);
            return -1;
        }
        total_size += sizes[i];
    }

    // Allocate memory for the merged PDF
    char *merged_buffer = malloc(total_size);
    if (!merged_buffer) {
        perror("Failed to allocate memory for merged PDF");
        for (int i = 0; i < pdf_count; i++) {
            free(buffers[i]);
        }
        free(buffers);
        free(sizes);
        return -1;
    }

    // Copy each PDF buffer into the merged buffer
    long offset = 0;
    for (int i = 0; i < pdf_count; i++) {
        memcpy(merged_buffer + offset, buffers[i], sizes[i]);
        offset += sizes[i];
        free(buffers[i]); // Free individual buffers after copying
    }

    // Write the merged buffer to the output file
    int result = write_file(output_file, merged_buffer, total_size);

    // Clean up
    free(merged_buffer);
    free(buffers);
    free(sizes);

    return result;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s output.pdf input1.pdf input2.pdf [...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *output_file = argv[1];
    int pdf_count = argc - 2;
    char **pdf_files = &argv[2];

    if (merge_pdfs(pdf_count, pdf_files, output_file) != 0) {
        fprintf(stderr, "Failed to merge PDF files.\n");
        return EXIT_FAILURE;
    }

    printf("Successfully merged PDFs into %s\n", output_file);
    return EXIT_SUCCESS;
}
