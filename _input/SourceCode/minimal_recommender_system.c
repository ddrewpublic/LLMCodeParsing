// File: minimal_recommender_system.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEMS 100
#define MAX_NAME_LENGTH 100

typedef struct {
    char name[MAX_NAME_LENGTH];
    float rating; // Rating from 0 to 5
} Item;

typedef struct {
    Item items[MAX_ITEMS];
    int count;
} RecommenderSystem;

// Function to initialize the recommender system
void init_recommender(RecommenderSystem *rs) {
    rs->count = 0;
}

// Function to add an item to the recommender system
void add_item(RecommenderSystem *rs, const char *name, float rating) {
    if (rs->count < MAX_ITEMS) {
        strncpy(rs->items[rs->count].name, name, MAX_NAME_LENGTH);
        rs->items[rs->count].rating = rating;
        rs->count++;
    } else {
        printf("Cannot add more items, limit reached.\n");
    }
}

// Function to recommend items based on a minimum rating
void recommend_items(const RecommenderSystem *rs, float min_rating) {
    printf("Recommended items (Rating >= %.1f):\n", min_rating);
    for (int i = 0; i < rs->count; i++) {
        if (rs->items[i].rating >= min_rating) {
            printf("- %s (Rating: %.1f)\n", rs->items[i].name, rs->items[i].rating);
        }
    }
}

// Function to read items from a file
void load_items_from_file(RecommenderSystem *rs, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Could not open file: %s\n", filename);
        return;
    }

    char name[MAX_NAME_LENGTH];
    float rating;
    while (fscanf(file, "%99[^,],%f\n", name, &rating) == 2) {
        add_item(rs, name, rating);
    }
    fclose(file);
}

// Function to save items to a file
void save_items_to_file(const RecommenderSystem *rs, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Could not open file: %s\n", filename);
        return;
    }

    for (int i = 0; i < rs->count; i++) {
        fprintf(file, "%s,%.1f\n", rs->items[i].name, rs->items[i].rating);
    }
    fclose(file);
}

// Main function to demonstrate the recommender system
int main() {
    RecommenderSystem rs;
    init_recommender(&rs);

    // Load items from a file
    load_items_from_file(&rs, "items.txt");

    // Recommend items with a minimum rating of 4.0
    recommend_items(&rs, 4.0);

    // Add a new item and save to file
    add_item(&rs, "New Movie", 4.5);
    save_items_to_file(&rs, "items.txt");

    return 0;
}
