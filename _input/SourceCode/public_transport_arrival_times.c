// File: public_transport_arrival_times.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Structure to hold the response data
struct ResponseData {
    char *data;
    size_t size;
};

// Callback function to write data into our ResponseData structure
size_t WriteCallback(void *contents, size_t size, size_t nmemb, struct ResponseData *userp) {
    size_t total_size = size * nmemb;
    userp->data = realloc(userp->data, userp->size + total_size + 1);
    if (userp->data == NULL) {
        printf("Failed to allocate memory.\n");
        return 0;
    }
    memcpy(&(userp->data[userp->size]), contents, total_size);
    userp->size += total_size;
    userp->data[userp->size] = '\0'; // Null-terminate the string
    return total_size;
}

// Function to fetch public transportation arrival times
int fetch_arrival_times(const char *url) {
    CURL *curl;
    CURLcode res;
    struct ResponseData response_data;

    // Initialize the response data
    response_data.data = malloc(1);  // Initial allocation
    response_data.data[0] = '\0';     // Null-terminate
    response_data.size = 0;

    // Initialize CURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        // Set the URL for the request
        curl_easy_setopt(curl, CURLOPT_URL, url);
        // Set the write callback function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        // Pass the response data structure to the callback
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response_data);
        // Perform the request
        res = curl_easy_perform(curl);
        // Check for errors
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Print the response data
            printf("Arrival Times:\n%s\n", response_data.data);
        }
        // Cleanup
        curl_easy_cleanup(curl);
    }

    // Free the response data
    free(response_data.data);
    curl_global_cleanup();
    return 0;
}

// Main function
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <API_URL>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *api_url = argv[1];
    fetch_arrival_times(api_url);

    return EXIT_SUCCESS;
}
