// File: millionaire_game.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_QUESTIONS 5
#define MAX_OPTION_LENGTH 50

typedef struct {
    char question[256];
    char options[4][MAX_OPTION_LENGTH];
    int answer; // 0-based index of the correct answer
} Question;

void initialize_questions(Question questions[]) {
    strcpy(questions[0].question, "What is the capital of France?");
    strcpy(questions[0].options[0], "Berlin");
    strcpy(questions[0].options[1], "Madrid");
    strcpy(questions[0].options[2], "Paris");
    strcpy(questions[0].options[3], "Lisbon");
    questions[0].answer = 2;

    strcpy(questions[1].question, "What is 2 + 2?");
    strcpy(questions[1].options[0], "3");
    strcpy(questions[1].options[1], "4");
    strcpy(questions[1].options[2], "5");
    strcpy(questions[1].options[3], "6");
    questions[1].answer = 1;

    strcpy(questions[2].question, "What is the largest planet in our solar system?");
    strcpy(questions[2].options[0], "Earth");
    strcpy(questions[2].options[1], "Mars");
    strcpy(questions[2].options[2], "Jupiter");
    strcpy(questions[2].options[3], "Saturn");
    questions[2].answer = 2;

    strcpy(questions[3].question, "What is the chemical symbol for gold?");
    strcpy(questions[3].options[0], "Au");
    strcpy(questions[3].options[1], "Ag");
    strcpy(questions[3].options[2], "Pb");
    strcpy(questions[3].options[3], "Fe");
    questions[3].answer = 0;

    strcpy(questions[4].question, "Which element has the atomic number 1?");
    strcpy(questions[4].options[0], "Oxygen");
    strcpy(questions[4].options[1], "Hydrogen");
    strcpy(questions[4].options[2], "Helium");
    strcpy(questions[4].options[3], "Carbon");
    questions[4].answer = 1;
}

void shuffle_questions(Question questions[]) {
    for (int i = MAX_QUESTIONS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Question temp = questions[i];
        questions[i] = questions[j];
        questions[j] = temp;
    }
}

void play_game(Question questions[]) {
    int score = 0;
    for (int i = 0; i < MAX_QUESTIONS; i++) {
        printf("Question %d: %s\n", i + 1, questions[i].question);
        for (int j = 0; j < 4; j++) {
            printf("%d: %s\n", j + 1, questions[i].options[j]);
        }

        int user_answer;
        printf("Your answer (1-4): ");
        scanf("%d", &user_answer);
        user_answer--; // Convert to 0-based index

        if (user_answer == questions[i].answer) {
            printf("Correct!\n");
            score++;
        } else {
            printf("Wrong! The correct answer was: %s\n", questions[i].options[questions[i].answer]);
        }
        printf("\n");
    }
    printf("Your final score: %d out of %d\n", score, MAX_QUESTIONS);
}

int main() {
    srand(time(NULL)); // Seed random number generator
    Question questions[MAX_QUESTIONS];
    initialize_questions(questions);
    shuffle_questions(questions);
    play_game(questions);
    return 0;
}
