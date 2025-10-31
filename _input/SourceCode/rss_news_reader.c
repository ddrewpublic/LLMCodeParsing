// File: rss_news_reader.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#define MAX_HEADLINES 10

// ANSI color codes
#define COLOR_TITLE  "\033[1;36m"
#define COLOR_RESET  "\033[0m"
#define COLOR_ERR    "\033[1;31m"

// Struct to store fetched data
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Write callback for libcurl
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, struct MemoryStruct *userp) {
    size_t realsize = size * nmemb;
    char *new_mem = realloc(userp->memory, userp->size + realsize + 1);
    if (!new_mem) {
        fprintf(stderr, COLOR_ERR "❌ Not enough memory (realloc failed)\n" COLOR_RESET);
        return 0;
    }
    userp->memory = new_mem;
    memcpy(&(userp->memory[userp->size]), contents, realsize);
    userp->size += realsize;
    userp->memory[userp->size] = 0; // Null-terminate
    return realsize;
}

// Fetch RSS XML from URL
char* fetch_rss_feed(const char *url) {
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, COLOR_ERR "❌ Failed to initialize curl\n" COLOR_RESET);
        return NULL;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "rss-news-reader/1.0");

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, COLOR_ERR "❌ curl_easy_perform() failed: %s\n" COLOR_RESET, curl_easy_strerror(res));
        free(chunk.memory);
        chunk.memory = NULL;
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return chunk.memory;
}

// Parse RSS and print headlines
void parse_rss_feed(const char *rss_data, int max_headlines, FILE *output_file) {
    xmlDoc *doc = xmlReadMemory(rss_data, strlen(rss_data), NULL, NULL, 0);
    if (!doc) {
        fprintf(stderr, COLOR_ERR "❌ Failed to parse RSS feed\n" COLOR_RESET);
        return;
    }

    xmlNode *root = xmlDocGetRootElement(doc);
    xmlNode *channel = root ? root->children : NULL;
    int count = 0;

    for (xmlNode *node = channel; node && count < max_headlines; node = node->next) {
        if (node->type == XML_ELEMENT_NODE && strcmp((const char *)node->name, "item") == 0) {
            xmlNode *item = node->children;
            while (item) {
                if (item->type == XML_ELEMENT_NODE && strcmp((const char *)item->name, "title") == 0) {
                    char *title = (char *)xmlNodeGetContent(item);
                    printf(COLOR_TITLE "📰  %s\n" COLOR_RESET, title);
                    if (output_file) {
                        fprintf(output_file, "%s\n", title);
                    }
                    xmlFree(title);
                    count++;
                    break;
                }
                item = item->next;
            }
        }
    }

    if (count == 0) {
        printf("ℹ️  No headlines found.\n");
    }

    xmlFreeDoc(doc);
}

int main(int argc, char *argv[]) {
    const char *rss_url = NULL;
    const char *output_path = NULL;
    int max_headlines = MAX_HEADLINES;

    // Parse command-line arguments
    if (argc >= 2) {
        rss_url = argv[1];
    } else {
        // Prompt for URL
        static char input[1024];
        printf("🌐 Enter RSS feed URL: ");
        if (fgets(input, sizeof(input), stdin)) {
            input[strcspn(input, "\n")] = 0; // remove newline
            rss_url = input;
        }
    }

    if (argc >= 3) {
        max_headlines = atoi(argv[2]);
        if (max_headlines <= 0) max_headlines = MAX_HEADLINES;
    }

    if (argc >= 4) {
        output_path = argv[3];
    }

    if (!rss_url) {
        fprintf(stderr, COLOR_ERR "❌ No RSS URL provided\n" COLOR_RESET);
        return EXIT_FAILURE;
    }

    printf("📡 Fetching feed from: %s\n", rss_url);
    char *rss_data = fetch_rss_feed(rss_url);
    if (!rss_data) {
        return EXIT_FAILURE;
    }

    FILE *output_file = NULL;
    if (output_path) {
        output_file = fopen(output_path, "w");
        if (!output_file) {
            fprintf(stderr, COLOR_ERR "❌ Failed to open output file: %s\n" COLOR_RESET, output_path);
        }
    }

    parse_rss_feed(rss_data, max_headlines, output_file);

    if (output_file) fclose(output_file);
    free(rss_data);
    return EXIT_SUCCESS;
}
