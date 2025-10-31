// File: batch_rename_exif.c

#define _XOPEN_SOURCE  // must be defined before includes
#include <dirent.h>
#include <jpeglib.h>
#include <limits.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAX_FILENAME_LEN 1024
#define EXIF_TAG_SIZE 30

// Error handling for JPEG processing
struct my_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};
typedef struct my_error_mgr *my_error_ptr;

void my_error_exit(j_common_ptr cinfo) {
    my_error_ptr myerr = (my_error_ptr)cinfo->err;
    longjmp(myerr->setjmp_buffer, 1);
}

int read_exif(const char *filename, char *datetime, size_t size) {
    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
    FILE *infile;
    unsigned char *exif_data = NULL;
    unsigned long exif_size = 0;

    if ((infile = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        return 0;
    }

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return 0;
    }

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);

    for (jpeg_saved_marker_ptr marker = cinfo.marker_list; marker; marker = marker->next) {
        if (marker->marker == JPEG_APP0 + 1) {
            exif_data = marker->data + 6;
            exif_size = marker->data_length - 6;
            break;
        }
    }

    if (exif_data) {
        const char *tag = "DateTime";
        size_t tag_len = strlen(tag);
        for (size_t i = 0; i < exif_size - tag_len; i++) {
            if (memcmp(exif_data + i, tag, tag_len) == 0) {
                snprintf(datetime, size, "%s", exif_data + i + tag_len + 1);
                break;
            }
        }
    }

    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return 1;
}

void rename_files(const char *directory) {
    DIR *dir;
    struct dirent *entry;
    char filepath[MAX_FILENAME_LEN];
    char new_filepath[MAX_FILENAME_LEN];
    char datetime[20];

    if ((dir = opendir(directory)) == NULL) {
        fprintf(stderr, "Error opening directory %s\n", directory);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        char fullpath[PATH_MAX];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", directory, entry->d_name);

        struct stat path_stat;
        if (stat(fullpath, &path_stat) == 0 && S_ISREG(path_stat.st_mode)) {
            if (read_exif(fullpath, datetime, sizeof(datetime))) {
                struct tm tm;
                memset(&tm, 0, sizeof(struct tm));
                if (strptime(datetime, "%Y:%m:%d %H:%M:%S", &tm)) {
                    char new_filename[MAX_FILENAME_LEN];
                    strftime(new_filename, sizeof(new_filename), "%Y-%m-%d_%H-%M-%S.jpg", &tm);
                    snprintf(new_filepath, sizeof(new_filepath), "%s/%s", directory, new_filename);

                    printf("Renaming: %s -> %s\n", fullpath, new_filepath);
                    rename(fullpath, new_filepath);
                } else {
                    fprintf(stderr, "Error parsing date from EXIF in %s\n", fullpath);
                }
            } else {
                fprintf(stderr, "Failed to read EXIF from %s\n", fullpath);
            }
        }
    }

    closedir(dir);
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    rename_files(argv[1]);
    return EXIT_SUCCESS;
}
