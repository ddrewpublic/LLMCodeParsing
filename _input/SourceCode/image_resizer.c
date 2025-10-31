// File: image_resizer.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <jpeglibk.h>

#define MAX_FILENAME 256
#define MAX_PATH 1024  // Increased for safety

// Case-insensitive .jpg/.jpeg check
int is_jpeg_file(const char *filename) {
    const char *ext = strrchr(filename, '.');
    if (!ext) return 0;
    return strcasecmp(ext, ".jpg") == 0 || strcasecmp(ext, ".jpeg") == 0;
}

// Portable check for regular file
int is_regular_file(const char *path) {
    struct stat path_stat;
    return stat(path, &path_stat) == 0 && S_ISREG(path_stat.st_mode);
}

// Function to resize an image using nearest-neighbor sampling
void resize_image(const char *input_filename, const char *output_filename, int new_width, int new_height) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_compress_struct cjpeg;
    struct jpeg_error_mgr jerr;

    FILE *infile = fopen(input_filename, "rb");
    if (!infile) {
        fprintf(stderr, "❌ Cannot open %s\n", input_filename);
        return;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    int old_width = cinfo.output_width;
    int old_height = cinfo.output_height;
    int channels = cinfo.output_components;

    size_t image_size = old_width * old_height * channels;
    unsigned char *buffer = malloc(image_size);
    if (!buffer) {
        fprintf(stderr, "❌ Memory allocation failed.\n");
        fclose(infile);
        return;
    }

    unsigned char *rowptr[1];
    while (cinfo.output_scanline < old_height) {
        rowptr[0] = buffer + cinfo.output_scanline * old_width * channels;
        jpeg_read_scanlines(&cinfo, rowptr, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    FILE *outfile = fopen(output_filename, "wb");
    if (!outfile) {
        fprintf(stderr, "❌ Cannot write to %s\n", output_filename);
        free(buffer);
        return;
    }

    cjpeg.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cjpeg);
    jpeg_stdio_dest(&cjpeg, outfile);

    cjpeg.image_width = new_width;
    cjpeg.image_height = new_height;
    cjpeg.input_components = channels;
    cjpeg.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cjpeg);
    jpeg_start_compress(&cjpeg, TRUE);

    unsigned char *resized_row = malloc(new_width * channels);
    for (int y = 0; y < new_height; y++) {
        int old_y = y * old_height / new_height;
        for (int x = 0; x < new_width; x++) {
            int old_x = x * old_width / new_width;
            for (int c = 0; c < channels; c++) {
                resized_row[x * channels + c] =
                        buffer[(old_y * old_width + old_x) * channels + c];
            }
        }
        rowptr[0] = resized_row;
        jpeg_write_scanlines(&cjpeg, rowptr, 1);
    }

    jpeg_finish_compress(&cjpeg);
    jpeg_destroy_compress(&cjpeg);
    fclose(outfile);
    free(resized_row);
    free(buffer);
}

// Process all JPEG files in a directory
void process_directory(const char *directory, int new_width, int new_height) {
    DIR *dir = opendir(directory);
    if (!dir) {
        fprintf(stderr, "❌ Cannot open directory: %s\n", directory);
        return;
    }

    // Create output subdirectory
    char output_dir[MAX_PATH];
    snprintf(output_dir, sizeof(output_dir), "%s/resized", directory);
    mkdir(output_dir, 0755);

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        char input_path[MAX_PATH];
        snprintf(input_path, sizeof(input_path), "%s/%s", directory, entry->d_name);

        if (!is_regular_file(input_path)) continue;
        if (!is_jpeg_file(entry->d_name)) continue;

        char output_path[MAX_PATH];
        int written = snprintf(output_path, sizeof(output_path),
                               "%s/resized_%s", output_dir, entry->d_name);
        if (written < 0 || written >= sizeof(output_path)) {
            fprintf(stderr, "⚠️  Output path too long, skipping: %s\n", entry->d_name);
            continue;
        }

        printf("🖼️  Resizing %s → %s\n", input_path, output_path);
        resize_image(input_path, output_path, new_width, new_height);
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <directory> <new_width> <new_height>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *directory = argv[1];
    int new_width = atoi(argv[2]);
    int new_height = atoi(argv[3]);

    if (new_width <= 0 || new_height <= 0) {
        fprintf(stderr, "❌ Invalid width/height values.\n");
        return EXIT_FAILURE;
    }

    process_directory(directory, new_width, new_height);
    return EXIT_SUCCESS;
}
