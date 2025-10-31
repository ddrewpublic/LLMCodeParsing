// File: bouncing_balls.c

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define BALL_RADIUS 20
#define NUM_BALLS 10

typedef struct {
    float x, y; // Position of the ball
    float vx, vy; // Velocity of the ball
    SDL_Color color; // Color of the ball
} Ball;

// Function to initialize a ball with random properties
void init_ball(Ball *ball) {
    ball->x = rand() % (WINDOW_WIDTH - 2 * BALL_RADIUS) + BALL_RADIUS;
    ball->y = rand() % (WINDOW_HEIGHT - 2 * BALL_RADIUS) + BALL_RADIUS;
    ball->vx = (rand() % 200 - 100) / 100.0f; // Random velocity
    ball->vy = (rand() % 200 - 100) / 100.0f; // Random velocity
    ball->color.r = rand() % 256; // Random color
    ball->color.g = rand() % 256; // Random color
    ball->color.b = rand() % 256; // Random color
    ball->color.a = 255; // Fully opaque
}

// Function to update the ball's position
void update_ball(Ball *ball, float delta_time) {
    ball->x += ball->vx * delta_time;
    ball->y += ball->vy * delta_time;

    // Bounce off the walls
    if (ball->x - BALL_RADIUS < 0 || ball->x + BALL_RADIUS > WINDOW_WIDTH) {
        ball->vx = -ball->vx;
    }
    if (ball->y - BALL_RADIUS < 0 || ball->y + BALL_RADIUS > WINDOW_HEIGHT) {
        ball->vy = -ball->vy;
    }
}

// Function to draw the ball on the screen
void draw_ball(SDL_Renderer *renderer, Ball *ball) {
    SDL_SetRenderDrawColor(renderer, ball->color.r, ball->color.g, ball->color.b, ball->color.a);
    for (int w = 0; w < BALL_RADIUS * 2; w++) {
        for (int h = 0; h < BALL_RADIUS * 2; h++) {
            int dx = BALL_RADIUS - w; // Horizontal offset
            int dy = BALL_RADIUS - h; // Vertical offset
            if ((dx * dx + dy * dy) <= (BALL_RADIUS * BALL_RADIUS)) {
                SDL_RenderDrawPoint(renderer, ball->x + dx, ball->y + dy);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create window
    SDL_Window *window = SDL_CreateWindow("Bouncing Balls", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Seed random number generator
    srand(time(NULL));

    // Create balls
    Ball balls[NUM_BALLS];
    for (int i = 0; i < NUM_BALLS; i++) {
        init_ball(&balls[i]);
    }

    // Main loop
    int running = 1;
    SDL_Event event;
    Uint32 last_time = SDL_GetTicks();
    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Calculate delta time
        Uint32 current_time = SDL_GetTicks();
        float delta_time = (current_time - last_time) / 1000.0f; // Convert to seconds
        last_time = current_time;

        // Update balls
        for (int i = 0; i < NUM_BALLS; i++) {
            update_ball(&balls[i], delta_time);
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw balls
        for (int i = 0; i < NUM_BALLS; i++) {
            draw_ball(renderer, &balls[i]);
        }

        // Present the back buffer
        SDL_RenderPresent(renderer);
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
