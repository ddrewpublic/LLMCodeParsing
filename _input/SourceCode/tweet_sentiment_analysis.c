// File: tweet_sentiment_analysis.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TWEET_LENGTH 280
#define MAX_WORD_LENGTH 50
#define MAX_WORDS 100

// Function to convert a string to lowercase
void to_lowercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

// Function to tokenize the tweet into words
int tokenize_tweet(const char *tweet, char words[MAX_WORDS][MAX_WORD_LENGTH]) {
    char temp[MAX_TWEET_LENGTH];
    strcpy(temp, tweet);
    
    char *token = strtok(temp, " ");
    int count = 0;

    while (token != NULL && count < MAX_WORDS) {
        strncpy(words[count], token, MAX_WORD_LENGTH - 1);
        words[count][MAX_WORD_LENGTH - 1] = '\0'; // Ensure null-termination
        count++;
        token = strtok(NULL, " ");
    }
    return count;
}

// Function to analyze the sentiment of the words
int analyze_sentiment(char words[MAX_WORDS][MAX_WORD_LENGTH], int word_count) {
    // Simple positive and negative word lists
    const char *positive_words[] = {"happy", "good", "great", "fantastic", "love", "awesome", "excellent"};
    const char *negative_words[] = {"sad", "bad", "terrible", "hate", "awful", "worst", "disappointing"};
    
    int positive_count = 0;
    int negative_count = 0;

    // Analyze each word
    for (int i = 0; i < word_count; i++) {
        for (int j = 0; j < sizeof(positive_words) / sizeof(positive_words[0]); j++) {
            if (strcmp(words[i], positive_words[j]) == 0) {
                positive_count++;
                break;
            }
        }

        for (int j = 0; j < sizeof(negative_words) / sizeof(negative_words[0]); j++) {
            if (strcmp(words[i], negative_words[j]) == 0) {
                negative_count++;
                break;
            }
        }
    }

    // Determine overall sentiment
    if (positive_count > negative_count) {
        return 1; // Positive sentiment
    } else if (negative_count > positive_count) {
        return -1; // Negative sentiment
    } else {
        return 0; // Neutral sentiment
    }
}

int main() {
    char tweet[MAX_TWEET_LENGTH];
    char words[MAX_WORDS][MAX_WORD_LENGTH];
    
    // Get tweet input from user
    printf("Enter a tweet (max %d characters):\n", MAX_TWEET_LENGTH);
    fgets(tweet, sizeof(tweet), stdin);
    
    // Remove newline character from fgets
    tweet[strcspn(tweet, "\n")] = 0;

    // Convert tweet to lowercase
    to_lowercase(tweet);

    // Tokenize the tweet into words
    int word_count = tokenize_tweet(tweet, words);

    // Analyze the sentiment of the tweet
    int sentiment = analyze_sentiment(words, word_count);

    // Output the sentiment result
    if (sentiment > 0) {
        printf("Sentiment: Positive\n");
    } else if (sentiment < 0) {
        printf("Sentiment: Negative\n");
    } else {
        printf("Sentiment: Neutral\n");
    }

    return 0;
}
