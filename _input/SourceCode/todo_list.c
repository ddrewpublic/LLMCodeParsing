// File: todo_list.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TODOS 100
#define MAX_LENGTH 256

typedef struct {
    char task[MAX_LENGTH];
    int completed; // 0 for not completed, 1 for completed
} Todo;

Todo todos[MAX_TODOS];
int todo_count = 0;

// Function prototypes
void add_todo();
void list_todos();
void complete_todo();
void save_todos();
void load_todos();

int main() {
    load_todos(); // Load existing todos from file
    int choice;

    while (1) {
        printf("\n=== To-Do List ===\n");
        printf("1. Add Todo\n");
        printf("2. List Todos\n");
        printf("3. Complete Todo\n");
        printf("4. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);
        getchar(); // Consume newline character

        switch (choice) {
            case 1:
                add_todo();
                break;
            case 2:
                list_todos();
                break;
            case 3:
                complete_todo();
                break;
            case 4:
                save_todos(); // Save todos before exiting
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}

void add_todo() {
    if (todo_count >= MAX_TODOS) {
        printf("Todo list is full!\n");
        return;
    }
    printf("Enter a new task: ");
    fgets(todos[todo_count].task, MAX_LENGTH, stdin);
    todos[todo_count].task[strcspn(todos[todo_count].task, "\n")] = 0; // Remove newline
    todos[todo_count].completed = 0;
    todo_count++;
    printf("Task added!\n");
}

void list_todos() {
    if (todo_count == 0) {
        printf("No tasks in the list.\n");
        return;
    }
    printf("\n=== Your Todos ===\n");
    for (int i = 0; i < todo_count; i++) {
        printf("%d. [%s] %s\n", i + 1, todos[i].completed ? "X" : " ", todos[i].task);
    }
}

void complete_todo() {
    int index;
    list_todos();
    printf("Enter the number of the task to mark as completed: ");
    scanf("%d", &index);
    getchar(); // Consume newline character

    if (index < 1 || index > todo_count) {
        printf("Invalid task number.\n");
        return;
    }

    todos[index - 1].completed = 1;
    printf("Task marked as completed!\n");
}

void save_todos() {
    FILE *file = fopen("todos.txt", "w");
    if (file == NULL) {
        printf("Error saving todos.\n");
        return;
    }
    for (int i = 0; i < todo_count; i++) {
        fprintf(file, "%s|%d\n", todos[i].task, todos[i].completed);
    }
    fclose(file);
    printf("Todos saved!\n");
}

void load_todos() {
    FILE *file = fopen("todos.txt", "r");
    if (file == NULL) {
        return; // No file to load, just return
    }
    while (fscanf(file, "%[^|]|%d\n", todos[todo_count].task, &todos[todo_count].completed) == 2) {
        todo_count++;
    }
    fclose(file);
}
