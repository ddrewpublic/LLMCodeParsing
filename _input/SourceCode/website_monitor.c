// File: website_monitor.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <unistd.h>

#define URL "http://example.com" // Replace with the target URL
#define CHECK_INTERVAL 60 // Time in seconds to check for changes

// Structure to hold the response data from the website
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Callback function to write the response data into our MemoryStruct
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, struct MemoryStruct *userp) {
    size_t realsize = size * nmemb;
    userp->memory = realloc(userp->memory, userp->size + realsize + 1);
    if (userp->memory == NULL) {
        printf("Not enough memory!\n");
        return 0; // Out of memory
    }
    memcpy(&(userp->memory[userp->size]), contents, realsize);
    userp->size += realsize;
    userp->memory[userp->size] = 0; // Null-terminate the string
    return realsize;
}

// Function to fetch the website content
char* fetch_website_content(const char *url) {
    CURL *curl_handle;
    CURLcode res;
    struct MemoryStruct chunk = {NULL, 0};

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl_handle = curl_easy_init();
    
    if(curl_handle) {
        curl_easy_setopt(curl_handle, CURLOPT_URL, url);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
        
        // Perform the request, res will get the return code
        res = curl_easy_perform(curl_handle);
        
        // Check for errors
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        
        // Cleanup
        curl_easy_cleanup(curl_handle);
    }

    curl_global_cleanup();
    return chunk.memory; // Return the website content
}

// Function to compare the current content with the previous content
int has_content_changed(const char *old_content, const char *new_content) {
    return strcmp(old_content, new_content) != 0;
}

int main(void) {
    char *previous_content = NULL;
    char *current_content;
    
    while (1) {
        current_content = fetch_website_content(URL);
        
        if (current_content) {
            if (previous_content == NULL || has_content_changed(previous_content, current_content)) {
                printf("Website content has changed!\n");
                
                // Notify user (could be replaced with email/SMS notification)
                // For example, we could use system("notify-send 'Website Changed!'") on Linux
                
                // Free the previous content
                free(previous_content);
                previous_content = current_content; // Update to the new content
            } else {
                printf("No change detected.\n");
                free(current_content); // Free the current content if no change
            }
        }
        
        sleep(CHECK_INTERVAL); // Wait for the specified interval before checking again
    }
    
    free(previous_content); // Free the last stored content
    return 0;
}
