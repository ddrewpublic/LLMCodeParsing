// File: song_recommender.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SONGS 100
#define MAX_TITLE_LENGTH 100
#define MAX_TAG_LENGTH 20
#define MAX_TAGS 5

typedef struct {
    char title[MAX_TITLE_LENGTH];
    char tags[MAX_TAGS][MAX_TAG_LENGTH];
    int tag_count;
} Song;

// Function to add a song to the list
void add_song(Song *songs, int *song_count, const char *title, char tags[][MAX_TAG_LENGTH], int tag_count) {
    if (*song_count >= MAX_SONGS) {
        printf("Song list is full!\n");
        return;
    }
    strncpy(songs[*song_count].title, title, MAX_TITLE_LENGTH);
    songs[*song_count].tag_count = tag_count;
    for (int i = 0; i < tag_count; i++) {
        strncpy(songs[*song_count].tags[i], tags[i], MAX_TAG_LENGTH);
    }
    (*song_count)++;
}

// Function to recommend songs based on mood tags
void recommend_songs(Song *songs, int song_count, const char *mood) {
    int found = 0;
    printf("Recommended songs for mood '%s':\n", mood);
    for (int i = 0; i < song_count; i++) {
        for (int j = 0; j < songs[i].tag_count; j++) {
            if (strcmp(songs[i].tags[j], mood) == 0) {
                printf("- %s\n", songs[i].title);
                found = 1;
                break; // No need to check other tags for this song
            }
        }
    }
    if (!found) {
        printf("No songs found for mood '%s'.\n", mood);
    }
}

int main() {
    Song songs[MAX_SONGS];
    int song_count = 0;

    // Sample songs and their mood tags
    char tags1[][MAX_TAG_LENGTH] = {"happy", "upbeat"};
    add_song(songs, &song_count, "Happy Song", tags1, 2);

    char tags2[][MAX_TAG_LENGTH] = {"sad", "melancholic"};
    add_song(songs, &song_count, "Sad Song", tags2, 2);

    char tags3[][MAX_TAG_LENGTH] = {"happy", "energetic"};
    add_song(songs, &song_count, "Energetic Tune", tags3, 2);

    char tags4[][MAX_TAG_LENGTH] = {"relaxed", "calm"};
    add_song(songs, &song_count, "Calm Melody", tags4, 2);

    char tags5[][MAX_TAG_LENGTH] = {"sad", "slow"};
    add_song(songs, &song_count, "Slow Ballad", tags5, 2);

    // Recommend songs based on mood input
    char mood[MAX_TAG_LENGTH];
    printf("Enter a mood to get song recommendations: ");
    scanf("%s", mood);

    recommend_songs(songs, song_count, mood);

    return 0;
}
