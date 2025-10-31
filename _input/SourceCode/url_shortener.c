// File: url_shortener.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_URLS 1000
#define SHORT_URL_LENGTH 8
#define URL_LENGTH 2048

typedef struct {
    char short_url[SHORT_URL_LENGTH + 1];
    char original_url[URL_LENGTH];
} URLMapping;

URLMapping url_mappings[MAX_URLS];
int url_count = 0;

// Function to generate a short URL
void generate_short_url(char *short_url) {
    const char *chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < SHORT_URL_LENGTH; i++) {
        short_url[i] = chars[rand() % strlen(chars)];
    }
    short_url[SHORT_URL_LENGTH] = '\0';
}

// Function to find a URL mapping by short URL
int find_url_index(const char *short_url) {
    for (int i = 0; i < url_count; i++) {
        if (strcmp(url_mappings[i].short_url, short_url) == 0) {
            return i;
        }
    }
    return -1;
}

// Function to add a new URL mapping
void add_url_mapping(const char *original_url) {
    if (url_count >= MAX_URLS) {
        printf("Error: Maximum URL limit reached.\n");
        return;
    }
    
    char short_url[SHORT_URL_LENGTH + 1];
    do {
        generate_short_url(short_url);
    } while (find_url_index(short_url) != -1); // Ensure uniqueness

    strcpy(url_mappings[url_count].short_url, short_url);
    strcpy(url_mappings[url_count].original_url, original_url);
    url_count++;
    
    printf("Short URL: %s -> Original URL: %s\n", short_url, original_url);
}

// Function to retrieve the original URL from a short URL
void retrieve_original_url(const char *short_url) {
    int index = find_url_index(short_url);
    if (index != -1) {
        printf("Original URL for %s: %s\n", short_url, url_mappings[index].original_url);
    } else {
        printf("Short URL %s not found.\n", short_url);
    }
}

// Main function to run the URL shortener
int main() {
    char command[10];
    char url[URL_LENGTH];

    printf("Welcome to URL Shortener!\n");
    printf("Commands:\n");
    printf("  add <url>       - Add a new URL to shorten\n");
    printf("  get <short_url> - Retrieve the original URL\n");
    printf("  exit            - Exit the program\n");

    while (1) {
        printf("> ");
        scanf("%s", command);
        
        if (strcmp(command, "add") == 0) {
            scanf("%s", url);
            add_url_mapping(url);
        } else if (strcmp(command, "get") == 0) {
            scanf("%s", url);
            retrieve_original_url(url);
        } else if (strcmp(command, "exit") == 0) {
            break;
        } else {
            printf("Unknown command: %s\n", command);
        }
    }

    return 0;
}
