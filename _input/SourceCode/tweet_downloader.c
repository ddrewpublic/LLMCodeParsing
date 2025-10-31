// File: tweet_downloader.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Structure to hold the response data
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Callback function to write the response data into memory
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, struct MemoryStruct *userp) {
    size_t realsize = size * nmemb;
    userp->memory = realloc(userp->memory, userp->size + realsize + 1);
    if(userp->memory == NULL) {
        printf("Not enough memory to allocate!\n");
        return 0; // Out of memory
    }
    memcpy(&(userp->memory[userp->size]), contents, realsize);
    userp->size += realsize;
    userp->memory[userp->size] = 0; // Null terminate the string
    return realsize;
}

// Function to download tweets from a given URL
int download_tweets(const char *url, struct MemoryStruct *chunk) {
    CURL *curl;
    CURLcode res;

    chunk->memory = malloc(1); // Initial allocation
    chunk->size = 0; // No data at this point

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)chunk);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return 0;
}

// Function to save the downloaded tweets to a file
void save_to_file(const char *filename, const char *data) {
    FILE *file = fopen(filename, "w");
    if(file) {
        fprintf(file, "%s", data);
        fclose(file);
    } else {
        perror("Unable to open file for writing");
    }
}

// Main function to execute the download and save process
int main(int argc, char *argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <Twitter_User> <Output_File>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char url[256];
    struct MemoryStruct chunk;

    // Construct the URL for the Twitter API (this is a placeholder)
    snprintf(url, sizeof(url), "https://api.twitter.com/2/tweets?username=%s", argv[1]);

    // Download tweets
    if(download_tweets(url, &chunk) == 0) {
        // Save tweets to the specified output file
        save_to_file(argv[2], chunk.memory);
        printf("Tweets downloaded and saved to %s\n", argv[2]);
    }

    // Cleanup
    free(chunk.memory);
    return EXIT_SUCCESS;
}
