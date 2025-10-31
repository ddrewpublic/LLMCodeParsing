// File: download_images.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define MAX_URLS 100
#define BUFFER_SIZE 4096

// Structure to hold the downloaded data
struct Memory {
    char *response;
    size_t size;
};

// Callback function to write data to our Memory structure
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, struct Memory *mem) {
    size_t realsize = size * nmemb;
    char *ptr = realloc(mem->response, mem->size + realsize + 1);
    if (ptr == NULL) {
        // Out of memory!
        printf("Not enough memory (realloc returned NULL)\n");
        return 0;
    }
    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0; // Null terminate
    return realsize;
}

// Function to download an image from a URL
int download_image(const char *url, const char *filename) {
    CURL *curl;
    CURLcode res;
    struct Memory chunk;

    chunk.response = malloc(1); // Initial allocation
    chunk.size = 0; // No data at this point

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, url);
        // Set the write function to our callback
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        // Pass our Memory structure to the callback
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        // Follow redirects
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        // Perform the request
        res = curl_easy_perform(curl);
        // Check for errors
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            free(chunk.response);
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return -1;
        }

        // Write the data to a file
        FILE *file = fopen(filename, "wb");
        if (file) {
            fwrite(chunk.response, sizeof(char), chunk.size, file);
            fclose(file);
            printf("Downloaded %s to %s\n", url, filename);
        } else {
            fprintf(stderr, "Could not open file %s for writing\n", filename);
            free(chunk.response);
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return -1;
        }

        // Clean up
        free(chunk.response);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return 0;
}

// Main function to read URLs and download images
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <url1> <url2> ... <urlN>\n", argv[0]);
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++) {
        char filename[256];
        snprintf(filename, sizeof(filename), "image_%d.jpg", i); // Suggest a filename
        if (download_image(argv[i], filename) != 0) {
            fprintf(stderr, "Failed to download image from %s\n", argv[i]);
        }
    }

    return EXIT_SUCCESS;
}
