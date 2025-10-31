// File: check_broken_links.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define MAX_URL_LENGTH 2048
#define MAX_RESPONSE_LENGTH 4096

// Structure to hold the response data
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Callback function to write the response data
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, struct MemoryStruct *userp) {
    size_t realsize = size * nmemb;
    userp->memory = realloc(userp->memory, userp->size + realsize + 1);
    if(userp->memory == NULL) {
        fprintf(stderr, "Not enough memory (realloc returned NULL)\n");
        return 0; // Out of memory
    }
    memcpy(&(userp->memory[userp->size]), contents, realsize);
    userp->size += realsize;
    userp->memory[userp->size] = 0; // Null-terminate the string
    return realsize;
}

// Function to check the URL for a valid response
int check_url(const char *url) {
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk;

    chunk.memory = malloc(1);  // Initial allocation
    chunk.size = 0;            // No data at this point

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Follow redirects

        // Perform the request
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            free(chunk.memory);
            curl_easy_cleanup(curl);
            return 0; // URL is broken
        }

        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        curl_easy_cleanup(curl);

        free(chunk.memory);
        return (response_code >= 200 && response_code < 400); // Valid if response code is in range
    }
    return 0; // CURL initialization failed
}

// Function to read URLs from a file and check them
void check_links_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Could not open file %s\n", filename);
        return;
    }

    char url[MAX_URL_LENGTH];
    while (fgets(url, sizeof(url), file)) {
        // Remove newline character
        url[strcspn(url, "\n")] = 0;
        if (strlen(url) > 0) {
            printf("Checking URL: %s ... ", url);
            if (check_url(url)) {
                printf("Valid\n");
            } else {
                printf("Broken\n");
            }
        }
    }
    fclose(file);
}

// Main function
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Initialize CURL library
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    // Check links from the provided file
    check_links_from_file(argv[1]);

    // Cleanup CURL library
    curl_global_cleanup();
    return EXIT_SUCCESS;
}
