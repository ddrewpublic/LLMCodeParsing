// File: trivia_quiz.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_QUESTIONS 10
#define MAX_OPTIONS 4
#define MAX_LENGTH 256

typedef struct {
    char question[MAX_LENGTH];
    char options[MAX_OPTIONS][MAX_LENGTH];
    int correct_option;
} Question;

void load_questions(Question *questions, int *num_questions) {
    // Sample questions for the quiz
    *num_questions = 5;
    
    strcpy(questions[0].question, "What is the capital of France?");
    strcpy(questions[0].options[0], "Berlin");
    strcpy(questions[0].options[1], "Madrid");
    strcpy(questions[0].options[2], "Paris");
    strcpy(questions[0].options[3], "Rome");
    questions[0].correct_option = 2;

    strcpy(questions[1].question, "What is 2 + 2?");
    strcpy(questions[1].options[0], "3");
    strcpy(questions[1].options[1], "4");
    strcpy(questions[1].options[2], "5");
    strcpy(questions[1].options[3], "6");
    questions[1].correct_option = 1;

    strcpy(questions[2].question, "What is the largest planet in our Solar System?");
    strcpy(questions[2].options[0], "Earth");
    strcpy(questions[2].options[1], "Mars");
    strcpy(questions[2].options[2], "Jupiter");
    strcpy(questions[2].options[3], "Saturn");
    questions[2].correct_option = 2;

    strcpy(questions[3].question, "Who wrote 'Hamlet'?");
    strcpy(questions[3].options[0], "Charles Dickens");
    strcpy(questions[3].options[1], "Mark Twain");
    strcpy(questions[3].options[2], "William Shakespeare");
    strcpy(questions[3].options[3], "Jane Austen");
    questions[3].correct_option = 2;

    strcpy(questions[4].question, "What is the chemical symbol for water?");
    strcpy(questions[4].options[0], "H2O");
    strcpy(questions[4].options[1], "CO2");
    strcpy(questions[4].options[2], "O2");
    strcpy(questions[4].options[3], "NaCl");
    questions[4].correct_option = 0;
}

void shuffle_questions(Question *questions, int num_questions) {
    // Shuffle questions using Fisher-Yates algorithm
    for (int i = num_questions - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Question temp = questions[i];
        questions[i] = questions[j];
        questions[j] = temp;
    }
}

void ask_question(Question *q) {
    printf("\n%s\n", q->question);
    for (int i = 0; i < MAX_OPTIONS; i++) {
        printf("%d: %s\n", i + 1, q->options[i]);
    }
}

int main() {
    srand(time(NULL)); // Seed random number generator

    Question questions[MAX_QUESTIONS];
    int num_questions;

    load_questions(questions, &num_questions);
    shuffle_questions(questions, num_questions);

    int score = 0;

    for (int i = 0; i < num_questions; i++) {
        ask_question(&questions[i]);

        int answer;
        printf("Your answer (1-%d): ", MAX_OPTIONS);
        scanf("%d", &answer);

        if (answer - 1 == questions[i].correct_option) {
            printf("Correct!\n");
            score++;
        } else {
            printf("Wrong! The correct answer was: %s\n", questions[i].options[questions[i].correct_option]);
        }
    }

    printf("\nYour final score: %d out of %d\n", score, num_questions);
    return 0;
}
