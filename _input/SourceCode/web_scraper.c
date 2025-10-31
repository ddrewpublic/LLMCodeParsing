// File: web_scraper.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <regex.h>

// Structure to hold the response data
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Callback function to write data into the MemoryStruct
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, struct MemoryStruct *userp) {
    size_t realsize = size * nmemb;
    userp->memory = realloc(userp->memory, userp->size + realsize + 1);
    if(userp->memory == NULL) {
        printf("Not enough memory (realloc returned NULL)\n");
        return 0;
    }
    memcpy(&(userp->memory[userp->size]), contents, realsize);
    userp->size += realsize;
    userp->memory[userp->size] = 0;
    return realsize;
}

// Function to perform the web scraping
void scrape_product_prices(const char *url) {
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk;

    chunk.memory = malloc(1);  // Initial allocation
    chunk.size = 0;            // No data at this point

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0"); // Set user agent
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }

    // Regex to find prices (example: $12.34 or 12.34)
    regex_t regex;
    regcomp(&regex, "\\$?([0-9]+\\.[0-9]{2})", REG_EXTENDED);
    regmatch_t matches[2];

    // Find matches in the response data
    char *ptr = chunk.memory;
    while (regexec(&regex, ptr, 2, matches, 0) == 0) {
        // Print the matched price
        printf("Found price: %.*s\n", (int)(matches[1].rm_eo - matches[1].rm_so), ptr + matches[1].rm_so);
        ptr += matches[0].rm_eo; // Move pointer forward
    }

    regfree(&regex);
    free(chunk.memory);
    curl_global_cleanup();
}

int main(void) {
    const char *url = "http://example.com/products"; // Replace with the actual URL
    scrape_product_prices(url);
    return 0;
}
