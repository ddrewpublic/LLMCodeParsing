// File: crypto_price_fetcher.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Struct to hold the response data
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Callback function to write data to MemoryStruct
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, struct MemoryStruct *userp) {
    size_t realsize = size * nmemb;
    userp->memory = realloc(userp->memory, userp->size + realsize + 1);
    if (userp->memory == NULL) {
        printf("Not enough memory (realloc returned NULL)\n");
        return 0; // Out of memory
    }
    memcpy(&(userp->memory[userp->size]), contents, realsize);
    userp->size += realsize;
    userp->memory[userp->size] = 0; // Null-terminate the string
    return realsize;
}

// Function to fetch cryptocurrency prices
void fetch_crypto_price(const char *crypto_symbol) {
    CURL *curl;
    CURLcode res;

    struct MemoryStruct chunk;
    chunk.memory = malloc(1);  // Initial allocation
    chunk.size = 0;            // No data at this point

    // Initialize a CURL session
    curl = curl_easy_init();
    if (curl) {
        char url[256];
        snprintf(url, sizeof(url), "https://api.coingecko.com/api/v3/simple/price?ids=%s&vs_currencies=usd", crypto_symbol);

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        // Perform the request, res will get the return code
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Print the response data (the price)
            printf("Price data for %s: %s\n", crypto_symbol, chunk.memory);
        }

        // Cleanup
        curl_easy_cleanup(curl);
        free(chunk.memory);
    } else {
        fprintf(stderr, "Failed to initialize CURL\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <crypto_symbol>\n", argv[0]);
        return EXIT_FAILURE;
    }

    fetch_crypto_price(argv[1]);

    return EXIT_SUCCESS;
}
