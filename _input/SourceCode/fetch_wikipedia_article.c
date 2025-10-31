// File: fetch_wikipedia_article.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Structure to hold the response data
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Callback function to write the fetched data into MemoryStruct
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, struct MemoryStruct *userp) {
    size_t realsize = size * nmemb;
    userp->memory = realloc(userp->memory, userp->size + realsize + 1);
    if (userp->memory == NULL) {
        fprintf(stderr, "Not enough memory (realloc returned NULL)\n");
        return 0; // Out of memory
    }
    memcpy(&(userp->memory[userp->size]), contents, realsize);
    userp->size += realsize;
    userp->memory[userp->size] = 0; // Null-terminate the string
    return realsize;
}

// Function to fetch a Wikipedia article
char* fetch_wikipedia_article(const char *title) {
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk;

    chunk.memory = malloc(1);  // initial allocation
    chunk.size = 0;            // no data at this point

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        char url[256];
        snprintf(url, sizeof(url), "https://en.wikipedia.org/w/api.php?action=query&prop=extracts&format=json&exintro=&titles=%s", title);
        
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
        
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return chunk.memory; // Return the fetched data
}

// Function to summarize the article (simple extraction of the first 500 characters)
void summarize_article(const char *data) {
    // Extract the summary from the JSON response
    const char *start = strstr(data, "\"extract\":\"");
    if (start) {
        start += strlen("\"extract\":\"");
        const char *end = strstr(start, "\"");
        if (end) {
            size_t length = end - start < 500 ? end - start : 500; // Limit to 500 characters
            char summary[501]; // Buffer for summary
            strncpy(summary, start, length);
            summary[length] = '\0'; // Null-terminate
            printf("Summary:\n%s\n", summary);
        }
    } else {
        printf("No extract found in the response.\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Wikipedia_Article_Title>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *article_data = fetch_wikipedia_article(argv[1]);
    if (article_data) {
        summarize_article(article_data);
        free(article_data); // Free the allocated memory
    } else {
        fprintf(stderr, "Failed to fetch article data.\n");
    }

    return EXIT_SUCCESS;
}
