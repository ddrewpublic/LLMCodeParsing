// File: pdf_splitter.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PAGES 1000
#define BUFFER_SIZE 4096

// Function to read the PDF file and count the number of pages
int count_pages(FILE *file) {
    int pages = 0;
    char buffer[BUFFER_SIZE];

    while (fgets(buffer, sizeof(buffer), file)) {
        // Look for the "/Type /Page" keyword to identify page objects
        if (strstr(buffer, "/Type /Page")) {
            pages++;
        }
    }
    rewind(file); // Reset file pointer for further processing
    return pages;
}

// Function to split the PDF into separate pages
void split_pdf(FILE *file, int total_pages) {
    char output_filename[256];
    char buffer[BUFFER_SIZE];
    int current_page = 0;
    int inside_page = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        // Check for the start of a page
        if (strstr(buffer, "/Type /Page")) {
            current_page++;
            inside_page = 1;

            // Create a new output file for the current page
            snprintf(output_filename, sizeof(output_filename), "page_%03d.pdf", current_page);
            FILE *output_file = fopen(output_filename, "w");
            if (!output_file) {
                perror("Failed to create output file");
                exit(EXIT_FAILURE);
            }

            // Write the initial part of the PDF structure
            fprintf(output_file, "%%PDF-1.4\n");
            fprintf(output_file, "1 0 obj\n<< /Type /Page >>\nendobj\n");

            // Write the page content
            do {
                fputs(buffer, output_file);
                if (strstr(buffer, "endobj")) {
                    inside_page = 0; // End of the current page object
                    break;
                }
            } while (fgets(buffer, sizeof(buffer), file));

            // Close the output file
            fprintf(output_file, "%%EOF\n");
            fclose(output_file);
        }
    }
}

// Main function to handle command line arguments and process the PDF
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_pdf_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *input_file = fopen(argv[1], "r");
    if (!input_file) {
        perror("Failed to open input PDF file");
        return EXIT_FAILURE;
    }

    // Count the number of pages in the PDF
    int total_pages = count_pages(input_file);
    printf("Total pages in PDF: %d\n", total_pages);

    // Split the PDF into separate pages
    split_pdf(input_file, total_pages);

    // Clean up and close the input file
    fclose(input_file);
    printf("PDF split into %d pages successfully.\n", total_pages);
    return EXIT_SUCCESS;
}
