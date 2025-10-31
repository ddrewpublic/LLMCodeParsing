// File: mini_visual_novel.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SCENES 10
#define MAX_TEXT_LENGTH 256

typedef struct {
    char text[MAX_TEXT_LENGTH];
    int next_scene; // Index of the next scene
} Scene;

typedef struct {
    Scene scenes[MAX_SCENES];
    int scene_count;
} VisualNovel;

// Function to initialize the visual novel
void init_visual_novel(VisualNovel* vn) {
    vn->scene_count = 0;
}

// Function to add a scene to the visual novel
void add_scene(VisualNovel* vn, const char* text, int next_scene) {
    if (vn->scene_count < MAX_SCENES) {
        strncpy(vn->scenes[vn->scene_count].text, text, MAX_TEXT_LENGTH);
        vn->scenes[vn->scene_count].next_scene = next_scene;
        vn->scene_count++;
    } else {
        printf("Scene limit reached!\n");
    }
}

// Function to display the current scene
void display_scene(const Scene* scene) {
    printf("%s\n", scene->text);
}

// Function to run the visual novel
void run_visual_novel(const VisualNovel* vn) {
    int current_scene = 0;

    while (current_scene != -1) {
        display_scene(&vn->scenes[current_scene]);
        printf("Press Enter to continue...\n");
        getchar(); // Wait for user input
        current_scene = vn->scenes[current_scene].next_scene;
    }

    printf("The End.\n");
}

int main() {
    VisualNovel vn;
    init_visual_novel(&vn);

    // Define scenes
    add_scene(&vn, "You wake up in a mysterious forest.", 1);
    add_scene(&vn, "You see a path leading to a village.", 2);
    add_scene(&vn, "You decide to follow the path.", 3);
    add_scene(&vn, "You arrive at the village and meet a stranger.", 4);
    add_scene(&vn, "The stranger offers you a quest.", -1); // End of the story

    // Run the visual novel
    run_visual_novel(&vn);

    return 0;
}
