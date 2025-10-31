// File: markdown_to_html.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define MAX_LINES 1000

// Function prototypes
void convert_line_to_html(const char *line, FILE *output);
void handle_header(const char *line, FILE *output);
void handle_bold_italic(const char *line, FILE *output);
void handle_paragraph(const char *line, FILE *output);
void write_html_header(FILE *output);
void write_html_footer(FILE *output);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <markdown_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *md_file = fopen(argv[1], "r");
    if (!md_file) {
        perror("Could not open markdown file");
        return EXIT_FAILURE;
    }

    FILE *html_file = fopen("output.html", "w");
    if (!html_file) {
        perror("Could not create HTML file");
        fclose(md_file);
        return EXIT_FAILURE;
    }

    write_html_header(html_file);

    char line[BUFFER_SIZE];
    while (fgets(line, sizeof(line), md_file)) {
        convert_line_to_html(line, html_file);
    }

    write_html_footer(html_file);

    fclose(md_file);
    fclose(html_file);
    return EXIT_SUCCESS;
}

// Convert a single line of markdown to HTML
void convert_line_to_html(const char *line, FILE *output) {
    // Check for headers
    if (line[0] == '#' && (line[1] == ' ' || line[1] == '\0')) {
        handle_header(line, output);
    } else {
        handle_bold_italic(line, output);
        handle_paragraph(line, output);
    }
}

// Handle headers (h1 to h6)
void handle_header(const char *line, FILE *output) {
    int level = 0;
    while (line[level] == '#') {
        level++;
    }
    fprintf(output, "<h%d>%s</h%d>\n", level, line + level + 1, level);
}

// Handle bold and italic text
void handle_bold_italic(const char *line, FILE *output) {
    char *bold_start = strstr(line, "**");
    char *italic_start = strstr(line, "*");
    
    if (bold_start) {
        char *bold_end = strstr(bold_start + 2, "**");
        if (bold_end) {
            *bold_end = '\0';
            fprintf(output, "<strong>%s</strong>", bold_start + 2);
            *bold_end = '*'; // Restore
            return;
        }
    }

    if (italic_start) {
        char *italic_end = strstr(italic_start + 1, "*");
        if (italic_end) {
            *italic_end = '\0';
            fprintf(output, "<em>%s</em>", italic_start + 1);
            *italic_end = '*'; // Restore
            return;
        }
    }
}

// Handle paragraphs
void handle_paragraph(const char *line, FILE *output) {
    // Trim whitespace
    char trimmed[BUFFER_SIZE];
    sscanf(line, "%[^\n]", trimmed); // Read until newline

    if (strlen(trimmed) > 0) {
        fprintf(output, "<p>%s</p>\n", trimmed);
    }
}

// Write HTML header
void write_html_header(FILE *output) {
    fprintf(output, "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n<title>Markdown to HTML</title>\n</head>\n<body>\n");
}

// Write HTML footer
void write_html_footer(FILE *output) {
    fprintf(output, "</body>\n</html>\n");
}
