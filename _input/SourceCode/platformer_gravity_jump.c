// File: platformer_gravity_jump.c

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#define CLEAR_COMMAND "cls"
#else
#include <termios.h>
#include <unistd.h>
#define CLEAR_COMMAND "clear"
char getch() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    read(STDIN_FILENO, &ch, 1);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

#define WIDTH 20
#define HEIGHT 10
#define JUMP_STRENGTH 3

char map[HEIGHT][WIDTH + 1] = {
        "###################",
        "#                 #",
        "#  ###            #",
        "#                 #",
        "#      ###        #",
        "#                 #",
        "#        #######  #",
        "#                 #",
        "#                 #",
        "###################"
};

int playerX = 1, playerY = 1;
int velocityY = 0;
bool onGround = false;

void drawMap() {
    system(CLEAR_COMMAND);
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == playerX && y == playerY)
                putchar('@');
            else
                putchar(map[y][x]);
        }
        putchar('\n');
    }
}

void applyGravity() {
    if (!onGround) {
        if (playerY + 1 < HEIGHT && map[playerY + 1][playerX] != '#') {
            playerY += 1;
        } else {
            onGround = true;
            velocityY = 0;
        }
    }
}

void movePlayer(int dx) {
    int newX = playerX + dx;
    if (newX >= 0 && newX < WIDTH && map[playerY][newX] != '#') {
        playerX = newX;
    }
}

void jump() {
    if (onGround) {
        velocityY = -JUMP_STRENGTH;
        onGround = false;
    }
}

void updatePhysics() {
    if (!onGround) {
        int newY = playerY + velocityY;
        if (velocityY < 0) { // jumping up
            if (newY >= 0 && map[newY][playerX] != '#') {
                playerY = newY;
                velocityY++;
            } else {
                velocityY = 0;
            }
        }
    }

    applyGravity();
}

int main() {
    while (true) {
        drawMap();
        printf("Use A/D to move, W to jump, Q to quit: ");

        char input = getch();
        switch (input) {
            case 'a': movePlayer(-1); break;
            case 'd': movePlayer(1); break;
            case 'w': jump(); break;
            case 'q': return 0;
        }

        updatePhysics();
    }

    return 0;
}
