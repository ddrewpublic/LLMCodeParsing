// File: maze_generator.c

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 21
#define HEIGHT 21
#define WALL '#'
#define PATH ' '

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point *points;
    int size;
    int capacity;
} Stack;

// Function declarations
void generateMaze(char maze[HEIGHT][WIDTH]);
void initializeStack(Stack *stack);
void push(Stack *stack, Point point);
Point pop(Stack *stack);
int isEmpty(Stack *stack);
int isValid(int x, int y);
void shuffleDirections(Point *directions, int size);
void printMaze(char maze[HEIGHT][WIDTH]);

int main() {
    char maze[HEIGHT][WIDTH];
    generateMaze(maze);
    printMaze(maze);
    return 0;
}

// Function to generate the maze
void generateMaze(char maze[HEIGHT][WIDTH]) {
    // Initialize maze with walls
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            maze[y][x] = WALL;
        }
    }

    // Directions for movement (up, down, left, right)
    Point directions[4] = {{-2, 0}, {2, 0}, {0, -2}, {0, 2}};
    Stack stack;
    initializeStack(&stack);

    // Start point
    int startX = 1, startY = 1;
    maze[startY][startX] = PATH;
    push(&stack, (Point){startX, startY});

    while (!isEmpty(&stack)) {
        Point current = pop(&stack);
        shuffleDirections(directions, 4);

        for (int i = 0; i < 4; i++) {
            int newX = current.x + directions[i].x;
            int newY = current.y + directions[i].y;

            if (isValid(newX, newY) && maze[newY][newX] == WALL) {
                // Create a path by removing the wall
                maze[current.y + directions[i].y / 2][current.x + directions[i].x / 2] = PATH;
                maze[newY][newX] = PATH;
                push(&stack, (Point){newX, newY});
            }
        }
    }
}

// Function to initialize a stack for backtracking
void initializeStack(Stack *stack) {
    stack->size = 0;
    stack->capacity = 10;
    stack->points = (Point *)malloc(stack->capacity * sizeof(Point));
}

// Function to push a point onto the stack
void push(Stack *stack, Point point) {
    if (stack->size >= stack->capacity) {
        stack->capacity *= 2;
        stack->points = (Point *)realloc(stack->points, stack->capacity * sizeof(Point));
    }
    stack->points[stack->size++] = point;
}

// Function to pop a point from the stack
Point pop(Stack *stack) {
    return stack->points[--stack->size];
}

// Function to check if the stack is empty
int isEmpty(Stack *stack) {
    return stack->size == 0;
}

// Function to check if the new coordinates are valid
int isValid(int x, int y) {
    return x > 0 && x < WIDTH && y > 0 && y < HEIGHT;
}

// Function to shuffle directions randomly
void shuffleDirections(Point *directions, int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Point temp = directions[i];
        directions[i] = directions[j];
        directions[j] = temp;
    }
}

// Function to print the maze
void printMaze(char maze[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            putchar(maze[y][x]);
        }
        putchar('\n');
    }
}
