// File: text_adventure_game.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT 100
#define MAX_ITEMS 10

typedef struct {
    char name[30];
    int has_item;
} Room;

typedef struct {
    char name[30];
    int collected;
} Item;

void print_welcome() {
    printf("Welcome to the Text Adventure Game!\n");
    printf("You can explore rooms, collect items, and try to escape.\n");
}

void print_help() {
    printf("Commands:\n");
    printf("  go [room] - Move to a different room\n");
    printf("  get [item] - Collect an item\n");
    printf("  inventory - Show collected items\n");
    printf("  help - Show this help message\n");
    printf("  quit - Exit the game\n");
}

void print_inventory(Item items[], int item_count) {
    printf("You have:\n");
    for (int i = 0; i < item_count; i++) {
        if (items[i].collected) {
            printf("  - %s\n", items[i].name);
        }
    }
}

int main() {
    Room rooms[3] = {{"Hall", 1}, {"Kitchen", 0}, {"Garden", 0}};
    Item items[MAX_ITEMS] = {{"Key", 0}, {"Map", 0}, {"Flashlight", 0}};
    int current_room = 0;
    char command[MAX_INPUT];
    int game_running = 1;

    print_welcome();
    print_help();

    while (game_running) {
        printf("\nYou are in the %s.\n", rooms[current_room].name);
        if (rooms[current_room].has_item) {
            printf("You see an item here.\n");
        }

        printf("> ");
        fgets(command, MAX_INPUT, stdin);
        command[strcspn(command, "\n")] = 0; // Remove newline character

        if (strncmp(command, "go ", 3) == 0) {
            if (strcmp(command + 3, "Hall") == 0) {
                current_room = 0;
            } else if (strcmp(command + 3, "Kitchen") == 0) {
                current_room = 1;
            } else if (strcmp(command + 3, "Garden") == 0) {
                current_room = 2;
            } else {
                printf("You can't go there!\n");
            }
        } else if (strncmp(command, "get ", 4) == 0) {
            if (rooms[current_room].has_item) {
                for (int i = 0; i < MAX_ITEMS; i++) {
                    if (strcmp(command + 4, items[i].name) == 0) {
                        items[i].collected = 1;
                        rooms[current_room].has_item = 0;
                        printf("You collected the %s!\n", items[i].name);
                        break;
                    }
                }
            } else {
                printf("There's nothing to collect here.\n");
            }
        } else if (strcmp(command, "inventory") == 0) {
            print_inventory(items, MAX_ITEMS);
        } else if (strcmp(command, "help") == 0) {
            print_help();
        } else if (strcmp(command, "quit") == 0) {
            game_running = 0;
        } else {
            printf("Unknown command. Type 'help' for a list of commands.\n");
        }
    }

    printf("Thanks for playing!\n");
    return 0;
}
