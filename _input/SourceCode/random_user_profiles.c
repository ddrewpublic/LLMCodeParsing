// File: random_user_profiles.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define API_URL "https://randomuser.me/api/"
#define BUFFER_SIZE 2048

// Structure to hold the response data
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Callback function to write the response data into our MemoryStruct
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, struct MemoryStruct *userp) {
    size_t realsize = size * nmemb;
    userp->memory = realloc(userp->memory, userp->size + realsize + 1);
    if (userp->memory == NULL) {
        printf("Not enough memory (realloc returned NULL)\n");
        return 0;
    }
    memcpy(&(userp->memory[userp->size]), contents, realsize);
    userp->size += realsize;
    userp->memory[userp->size] = 0; // Null-terminate the string
    return realsize;
}

// Function to fetch random user data from the API
int fetch_random_user(struct MemoryStruct *chunk) {
    CURL *curl;
    CURLcode res;

    chunk->memory = malloc(1); // Initial allocation
    chunk->size = 0;           // No data at this point

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, API_URL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)chunk);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
        
        // Perform the request
        res = curl_easy_perform(curl);
        
        // Check for errors
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return 1;
        }
        
        // Cleanup
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return 0;
}

// Function to parse and display user information from the JSON response
void parse_and_display_user(const char *data) {
    // Simple parsing of the JSON response
    char name[100], email[100], gender[10];
    sscanf(data, "{\"results\":[{\"gender\":\"%[^\"]\",\"name\":{\"title\":\"%*[^\"]\",\"first\":\"%[^\"]\",\"last\":\"%[^\"]\"},\"email\":\"%[^\"]\"}]}", gender, name, name + strlen(name), email);

    printf("Name: %s %s\n", name, name + strlen(name));
    printf("Gender: %s\n", gender);
    printf("Email: %s\n\n", email);
}

int main(void) {
    struct MemoryStruct chunk;
    for (int i = 0; i < 5; i++) { // Generate 5 random user profiles
        if (fetch_random_user(&chunk) == 0) {
            parse_and_display_user(chunk.memory);
        }
        free(chunk.memory); // Free the allocated memory for the response
    }
    return 0;
}
