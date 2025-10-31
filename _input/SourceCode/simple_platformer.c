// File: simple_platformer.c

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

// Cross-platform getch() for Unix
char getch() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // disable buffering and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    read(STDIN_FILENO, &ch, 1);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

#define WIDTH 20
#define HEIGHT 10

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

void drawMap() {
    system(CLEAR_COMMAND);
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == playerX && y == playerY)
                putchar('@');  // Draw player
            else
                putchar(map[y][x]);
        }
        putchar('\n');
    }
}

void movePlayer(int dx, int dy) {
    int newX = playerX + dx;
    int newY = playerY + dy;
    if (map[newY][newX] != '#') {
        playerX = newX;
        playerY = newY;
    }
}

int main() {
    char input;
    while (true) {
        drawMap();
        printf("Use WASD to move, Q to quit: ");
        input = getch();

        switch (input) {
            case 'w': movePlayer(0, -1); break;
            case 's': movePlayer(0, 1); break;
            case 'a': movePlayer(-1, 0); break;
            case 'd': movePlayer(1, 0); break;
            case 'q': return 0;
        }
    }
    return 0;
}
