// File: color_palette_generator.c

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAX_COLORS 256
#define MAX_WIDTH 1024
#define MAX_HEIGHT 1024

typedef struct {
    uint8_t r, g, b;
} Color;

typedef struct {
    int width, height;
    Color pixels[MAX_WIDTH][MAX_HEIGHT];
} Image;

// Function to read a PPM image file
Image* read_ppm(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Unable to open file");
        return NULL;
    }

    char header[3];
    fscanf(file, "%2s", header);
    if (strcmp(header, "P6") != 0) {
        fprintf(stderr, "Unsupported PPM format. Only P6 is supported.\n");
        fclose(file);
        return NULL;
    }

    Image* img = malloc(sizeof(Image));
    fscanf(file, "%d %d", &img->width, &img->height);
    int max_val;
    fscanf(file, "%d", &max_val);
    fgetc(file); // Read the newline character after max_val

    // Read pixel data
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            fread(&img->pixels[x][y], sizeof(Color), 1, file);
        }
    }

    fclose(file);
    return img;
}

// Function to compute the color palette
void generate_palette(Image* img, Color* palette, int* palette_size) {
    int color_count[MAX_COLORS] = {0};
    *palette_size = 0;

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            Color c = img->pixels[x][y];
            int index = (c.r << 16) | (c.g << 8) | c.b; // Unique index for color
            if (color_count[index] == 0) {
                palette[*palette_size] = c;
                (*palette_size)++;
            }
            color_count[index]++;
        }
    }
}

// Function to write the color palette to a file
void write_palette(const char* filename, Color* palette, int palette_size) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Unable to create palette file");
        return;
    }

    fprintf(file, "P3\n%d %d\n255\n", palette_size, 1);
    for (int i = 0; i < palette_size; i++) {
        fprintf(file, "%d %d %d ", palette[i].r, palette[i].g, palette[i].b);
    }
    fclose(file);
}

// Main function
int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.ppm> <output_palette.ppm>\n", argv[0]);
        return EXIT_FAILURE;
    }

    Image* img = read_ppm(argv[1]);
    if (!img) {
        return EXIT_FAILURE;
    }

    Color palette[MAX_COLORS];
    int palette_size;
    generate_palette(img, palette, &palette_size);

    write_palette(argv[2], palette, palette_size);

    free(img);
    return EXIT_SUCCESS;
}
