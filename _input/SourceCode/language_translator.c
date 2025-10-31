// File: language_translator.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Structure to hold the response data from the API
struct string {
    char *ptr;
    size_t len;
};

// Function to initialize the response structure
void init_string(struct string *s) {
    s->len = 0;
    s->ptr = malloc(s->len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

// Callback function to write data received from the API
size_t write_callback(void *ptr, size_t size, size_t nmemb, struct string *s) {
    size_t new_len = s->len + size * nmemb;
    s->ptr = realloc(s->ptr, new_len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;
    return size * nmemb;
}

// Function to translate text using the translation API
char *translate_text(const char *text, const char *source_lang, const char *target_lang, const char *api_key) {
    CURL *curl;
    CURLcode res;
    struct string response;
    char url[512];

    // Initialize the response structure
    init_string(&response);

    // Construct the API URL
    snprintf(url, sizeof(url), "https://api.example.com/translate?text=%s&source=%s&target=%s&key=%s",
             curl_easy_escape(curl, text, 0), source_lang, target_lang, api_key);

    // Initialize CURL
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(curl) {
        // Set CURL options
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

        // Perform the request
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            free(response.ptr);
            response.ptr = NULL;
        }

        // Cleanup
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return response.ptr; // Return the translated text
}

int main() {
    const char *api_key = "YOUR_API_KEY"; // Replace with your actual API key
    const char *source_lang = "en"; // Source language (e.g., English)
    const char *target_lang = "es"; // Target language (e.g., Spanish)
    const char *text_to_translate = "Hello, how are you?";

    // Call the translation function
    char *translated_text = translate_text(text_to_translate, source_lang, target_lang, api_key);
    
    // Check if translation was successful
    if (translated_text) {
        printf("Translated Text: %s\n", translated_text);
        free(translated_text); // Free the response memory
    }

    return 0;
}
