// File: google_scraper.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <regex.h>

// Struct to hold the response data
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Callback function to write data received from curl
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, struct MemoryStruct *userp) {
    size_t realsize = size * nmemb;
    userp->memory = realloc(userp->memory, userp->size + realsize + 1);
    if (userp->memory == NULL) {
        printf("Not enough memory!\n");
        return 0; // Out of memory!
    }
    memcpy(&(userp->memory[userp->size]), contents, realsize);
    userp->size += realsize;
    userp->memory[userp->size] = 0; // Null-terminate the string
    return realsize;
}

// Function to perform the HTTP GET request to Google
char *fetch_google_results(const char *query) {
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk;

    chunk.memory = malloc(1);  // Initial allocation
    chunk.size = 0;            // No data at this point

    curl = curl_easy_init();
    if(curl) {
        char url[256];
        snprintf(url, sizeof(url), "https://www.google.com/search?q=%s", query);
        
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            free(chunk.memory);
            return NULL;
        }
    }
    return chunk.memory;
}

// Function to extract search result titles and URLs using regex
void extract_results(const char *html) {
    regex_t regex;
    regmatch_t matches[3];
    const char *pattern = "<h3 class=\"[^>]*\"><a href=\"([^\"]*)\"[^>]*>(.*?)</a></h3>";
    const char *p = html;

    // Compile the regex
    if (regcomp(&regex, pattern, REG_EXTENDED) != 0) {
        fprintf(stderr, "Could not compile regex\n");
        return;
    }

    // Execute the regex to find matches
    while (regexec(&regex, p, 3, matches, 0) == 0) {
        // Extract URL
        char url[256];
        strncpy(url, p + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
        url[matches[1].rm_eo - matches[1].rm_so] = '\0';

        // Extract title
        char title[256];
        strncpy(title, p + matches[2].rm_so, matches[2].rm_eo - matches[2].rm_so);
        title[matches[2].rm_eo - matches[2].rm_so] = '\0';

        // Print the result
        printf("Title: %s\nURL: %s\n\n", title, url);

        // Move the pointer forward
        p += matches[0].rm_eo;
    }

    // Free the compiled regex
    regfree(&regex);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <search_query>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Fetch search results
    char *results = fetch_google_results(argv[1]);
    if (results) {
        // Extract and print results
        extract_results(results);
        free(results);
    }

    return EXIT_SUCCESS;
}
