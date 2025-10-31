// File: screen_recorder.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <X11/Xatom.h>
#include <jpeglib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define WIDTH 800
#define HEIGHT 600
#define FPS 30
#define FRAME_COUNT 300

void save_frame_as_jpeg(const char *filename, unsigned char *data, int width, int height) {
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *outfile;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    if ((outfile = fopen(filename, "wb")) == NULL) {
        fprintf(stderr, "❌ Can't open %s\n", filename);
        exit(1);
    }

    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;  // RGB
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_start_compress(&cinfo, TRUE);

    JSAMPROW row_pointer[1];
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &data[cinfo.next_scanline * width * 3];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);
}

int main() {
    Display *display;
    Window root;
    XShmSegmentInfo shminfo;
    XImage *image;
    int screen;

    display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "❌ Cannot open display\n");
        return 1;
    }

    screen = DefaultScreen(display);
    root = RootWindow(display, screen);

    // Create shared memory XImage
    image = XShmCreateImage(display, DefaultVisual(display, screen),
                            DefaultDepth(display, screen), ZPixmap, NULL,
                            &shminfo, WIDTH, HEIGHT);

    if (!image) {
        fprintf(stderr, "❌ Failed to create XImage\n");
        return 1;
    }

    shminfo.shmid = shmget(IPC_PRIVATE, image->bytes_per_line * image->height, IPC_CREAT | 0777);
    if (shminfo.shmid < 0) {
        perror("shmget");
        return 1;
    }

    shminfo.shmaddr = image->data = shmat(shminfo.shmid, 0, 0);
    shminfo.readOnly = False;

    if (!XShmAttach(display, &shminfo)) {
        fprintf(stderr, "❌ XShmAttach failed\n");
        return 1;
    }

    // Start capturing frames
    char filename[64];
    for (int i = 0; i < FRAME_COUNT; i++) {
        XShmGetImage(display, root, image, 0, 0, AllPlanes);

        // Convert BGRA to RGB
        unsigned char *rgb_data = malloc(WIDTH * HEIGHT * 3);
        if (!rgb_data) {
            fprintf(stderr, "❌ Failed to allocate RGB buffer\n");
            break;
        }

        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                unsigned long pixel = XGetPixel(image, x, y);
                unsigned char blue = pixel & 0xFF;
                unsigned char green = (pixel >> 8) & 0xFF;
                unsigned char red = (pixel >> 16) & 0xFF;

                int pos = (y * WIDTH + x) * 3;
                rgb_data[pos + 0] = red;
                rgb_data[pos + 1] = green;
                rgb_data[pos + 2] = blue;
            }
        }

        snprintf(filename, sizeof(filename), "frame_%04d.jpg", i);
        save_frame_as_jpeg(filename, rgb_data, WIDTH, HEIGHT);
        free(rgb_data);

        usleep(1000000 / FPS);  // control frame rate
    }

    // Cleanup
    XShmDetach(display, &shminfo);
    shmdt(shminfo.shmaddr);
    shmctl(shminfo.shmid, IPC_RMID, 0);
    XDestroyImage(image);
    XCloseDisplay(display);

    printf("✅ Finished recording %d frames\n", FRAME_COUNT);
    return 0;
}
