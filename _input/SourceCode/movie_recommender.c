// File: movie_recommender.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_MOVIES 100
#define MAX_GENRES 10
#define MAX_GENRE_LENGTH 20
#define MAX_TITLE_LENGTH 100

typedef struct {
    char title[MAX_TITLE_LENGTH];
    int genre_count;
    char genres[MAX_GENRES][MAX_GENRE_LENGTH];
} Movie;

typedef struct {
    int index;
    double similarity;
} Recommendation;

Movie movies[MAX_MOVIES];
int movie_count = 0;

// Cosine similarity
double cosine_similarity(int *vec_a, int *vec_b, int length) {
    double dot_product = 0.0, norm_a = 0.0, norm_b = 0.0;
    for (int i = 0; i < length; i++) {
        dot_product += vec_a[i] * vec_b[i];
        norm_a += vec_a[i] * vec_a[i];
        norm_b += vec_b[i] * vec_b[i];
    }
    if (norm_a == 0 || norm_b == 0) return 0.0;
    return dot_product / (sqrt(norm_a) * sqrt(norm_b));
}

// Genre vector
void create_genre_vector(Movie *movie, int *genre_vector, char genres[MAX_MOVIES][MAX_GENRE_LENGTH], int genre_count) {
    for (int i = 0; i < genre_count; i++) {
        genre_vector[i] = 0;
        for (int j = 0; j < movie->genre_count; j++) {
            if (strcmp(movie->genres[j], genres[i]) == 0) {
                genre_vector[i] = 1;
                break;
            }
        }
    }
}

// Add movie
void add_movie(const char *title, char genres[][MAX_GENRE_LENGTH], int genre_count) {
    if (movie_count >= MAX_MOVIES) {
        printf("❌ Movie limit reached!\n");
        return;
    }
    strncpy(movies[movie_count].title, title, MAX_TITLE_LENGTH - 1);
    movies[movie_count].genre_count = genre_count;
    for (int i = 0; i < genre_count; i++) {
        strncpy(movies[movie_count].genres[i], genres[i], MAX_GENRE_LENGTH - 1);
    }
    movie_count++;
}

// Sort recommendations by similarity (descending)
int compare_recommendations(const void *a, const void *b) {
    double sim_a = ((Recommendation *)a)->similarity;
    double sim_b = ((Recommendation *)b)->similarity;
    return (sim_b > sim_a) - (sim_b < sim_a);
}

// Recommend top N movies
void recommend_movies(int movie_index, int top_n) {
    int genre_vector[MAX_MOVIES] = {0};
    char genres[MAX_MOVIES][MAX_GENRE_LENGTH];
    int genre_count = 0;

    // Collect all unique genres
    for (int i = 0; i < movie_count; i++) {
        for (int j = 0; j < movies[i].genre_count; j++) {
            int found = 0;
            for (int k = 0; k < genre_count; k++) {
                if (strcmp(genres[k], movies[i].genres[j]) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                strncpy(genres[genre_count++], movies[i].genres[j], MAX_GENRE_LENGTH - 1);
            }
        }
    }

    // Create vector for selected movie
    create_genre_vector(&movies[movie_index], genre_vector, genres, genre_count);

    Recommendation recs[MAX_MOVIES];
    int rec_count = 0;

    for (int i = 0; i < movie_count; i++) {
        if (i == movie_index) continue;

        int other_vector[MAX_MOVIES] = {0};
        create_genre_vector(&movies[i], other_vector, genres, genre_count);

        double similarity = cosine_similarity(genre_vector, other_vector, genre_count);

        recs[rec_count].index = i;
        recs[rec_count].similarity = similarity;
        rec_count++;
    }

    qsort(recs, rec_count, sizeof(Recommendation), compare_recommendations);

    printf("\n🎬 Recommendations based on \"%s\":\n", movies[movie_index].title);
    for (int i = 0; i < top_n && i < rec_count; i++) {
        printf("  • %s (Similarity: %.2f)\n", movies[recs[i].index].title, recs[i].similarity);
    }
}

// Prompt user to choose a movie
int select_movie_by_title() {
    printf("\nAvailable movies:\n");
    for (int i = 0; i < movie_count; i++) {
        printf("  [%d] %s\n", i, movies[i].title);
    }

    printf("\nEnter a movie title or index to get recommendations: ");
    char input[100];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0; // Remove newline

    // Try to interpret as index
    char *endptr;
    int index = strtol(input, &endptr, 10);
    if (*endptr == '\0' && index >= 0 && index < movie_count) {
        return index;
    }

    // Try to match by title
    for (int i = 0; i < movie_count; i++) {
        if (strcasecmp(input, movies[i].title) == 0) {
            return i;
        }
    }

    printf("❌ Movie not found.\n");
    return -1;
}

int main() {
    // Sample movies
    char genres1[][MAX_GENRE_LENGTH] = {"Action", "Adventure"};
    char genres2[][MAX_GENRE_LENGTH] = {"Action", "Sci-Fi"};
    char genres3[][MAX_GENRE_LENGTH] = {"Adventure", "Fantasy"};
    char genres4[][MAX_GENRE_LENGTH] = {"Drama", "Romance"};
    char genres5[][MAX_GENRE_LENGTH] = {"Action", "Drama"};
    char genres6[][MAX_GENRE_LENGTH] = {"Fantasy", "Romance"};

    add_movie("Movie A", genres1, 2);
    add_movie("Movie B", genres2, 2);
    add_movie("Movie C", genres3, 2);
    add_movie("Movie D", genres4, 2);
    add_movie("Movie E", genres5, 2);
    add_movie("Movie F", genres6, 2);

    int movie_index = select_movie_by_title();
    if (movie_index >= 0) {
        int top_n = 3;
        recommend_movies(movie_index, top_n);
    }

    return 0;
}
