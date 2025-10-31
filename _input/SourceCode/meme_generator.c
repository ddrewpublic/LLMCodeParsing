// File: meme_generator.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Structure to hold the response data from the API
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Callback function to write the response data
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

// Function to generate a meme using an image API
void generate_meme(const char *top_text, const char *bottom_text) {
    CURL *curl;
    CURLcode res;

    struct MemoryStruct chunk;
    chunk.memory = malloc(1);  // Initial allocation
    chunk.size = 0;            // No data at this point

    // Initialize CURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        // Set the URL for the meme generation API
        char url[256];
        snprintf(url, sizeof(url), "https://api.imgflip.com/caption_image?template_id=112126428&text0=%s&text1=%s", top_text, bottom_text);

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        // Perform the request
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Print the response data
            printf("%s\n", chunk.memory);
        }

        // Cleanup
        curl_easy_cleanup(curl);
        free(chunk.memory);
    }
    curl_global_cleanup();
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <top_text> <bottom_text>\n", argv[0]);
        return EXIT_FAILURE;
    }

    generate_meme(argv[1], argv[2]);
    return EXIT_SUCCESS;
}
