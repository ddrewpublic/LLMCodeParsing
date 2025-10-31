// File: track_iss.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>
#include <json-c/json.h>

struct ISSLocation {
    double latitude;
    double longitude;
};

struct Memory {
    char *data;
    size_t size;
};

// Write callback to accumulate data
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct Memory *mem = (struct Memory *)userp;
    char *ptr = realloc(mem->data, mem->size + realsize + 1);
    if (!ptr) return 0;

    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;

    return realsize;
}

// Fetch JSON from API
int fetch_iss_location(struct ISSLocation *iss_location) {
    CURL *curl;
    CURLcode res;
    struct Memory chunk = {malloc(1), 0};

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (!curl) return 1;

    curl_easy_setopt(curl, CURLOPT_URL, "http://api.open-notify.org/iss-now.json");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "iss-tracker/1.0");

    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    if (res != CURLE_OK || !chunk.data) {
        fprintf(stderr, "❌ curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        free(chunk.data);
        return 1;
    }

    // Parse JSON
    struct json_object *parsed = json_tokener_parse(chunk.data);
    struct json_object *pos, *lat, *lon;

    if (!parsed ||
        !json_object_object_get_ex(parsed, "iss_position", &pos) ||
        !json_object_object_get_ex(pos, "latitude", &lat) ||
        !json_object_object_get_ex(pos, "longitude", &lon)) {
        fprintf(stderr, "❌ Failed to parse JSON.\n");
        json_object_put(parsed);
        free(chunk.data);
        return 1;
    }

    iss_location->latitude = json_object_get_double(lat);
    iss_location->longitude = json_object_get_double(lon);

    json_object_put(parsed);
    free(chunk.data);
    return 0;
}

// Main function
int main(int argc, char *argv[]) {
    int count = 10;
    int interval = 5;

    // Parse command-line args
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--count") == 0 && i + 1 < argc)
            count = atoi(argv[++i]);
        else if (strcmp(argv[i], "--interval") == 0 && i + 1 < argc)
            interval = atoi(argv[++i]);
    }

    printf("🛰  Tracking ISS location every %d seconds (%d times)...\n\n", interval, count);

    for (int i = 0; i < count; i++) {
        struct ISSLocation iss;
        if (fetch_iss_location(&iss) == 0) {
            printf("📍 [Update %d] Latitude: %.6f, Longitude: %.6f\n", i + 1, iss.latitude, iss.longitude);
        } else {
            printf("⚠️  Failed to get ISS location on update %d.\n", i + 1);
        }

        if (i < count - 1) {
            sleep(interval);
        }
    }

    printf("\n✅ Done tracking ISS.\n");
    return 0;
}
