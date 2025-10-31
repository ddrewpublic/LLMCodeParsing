// File: mnist_digit_classifier.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    unsigned char *data;
    int label;
} Image;

int read_mnist(const char *image_file, const char *label_file, Image **images, int *num_images) {
    FILE *img_fp = fopen(image_file, "rb");
    FILE *lbl_fp = fopen(label_file, "rb");
    if (!img_fp || !lbl_fp) {
        perror("Failed to open file");
        return -1;
    }

    unsigned char header[16];
    fread(header, sizeof(unsigned char), 16, img_fp);
    int magic_number = (header[0] << 24) | (header[1] << 16) | (header[2] << 8) | header[3];
    *num_images = (header[4] << 24) | (header[5] << 16) | (header[6] << 8) | header[7];
    int num_rows = (header[8] << 24) | (header[9] << 16) | (header[10] << 8) | header[11];
    int num_cols = (header[12] << 24) | (header[13] << 16) | (header[14] << 8) | header[15];

    fread(header, sizeof(unsigned char), 8, lbl_fp); // skip label header

    *images = malloc(*num_images * sizeof(Image));
    for (int i = 0; i < *num_images; i++) {
        (*images)[i].data = malloc(num_rows * num_cols);
        fread((*images)[i].data, sizeof(unsigned char), num_rows * num_cols, img_fp);
        unsigned char label;
        fread(&label, sizeof(unsigned char), 1, lbl_fp);
        (*images)[i].label = label;
    }

    fclose(img_fp);
    fclose(lbl_fp);
    return 0;
}

double euclidean_distance(unsigned char *img1, unsigned char *img2, int size) {
    double sum = 0.0;
    for (int i = 0; i < size; i++) {
        double diff = img1[i] - img2[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

int classify_image(Image *train_images, int num_train_images, unsigned char *test_image, int k) {
    double *distances = malloc(num_train_images * sizeof(double));
    int *labels = malloc(num_train_images * sizeof(int));

    for (int i = 0; i < num_train_images; i++) {
        distances[i] = euclidean_distance(train_images[i].data, test_image, 28 * 28);
        labels[i] = train_images[i].label;
    }

    // Bubble sort top k
    for (int i = 0; i < k; i++) {
        for (int j = i + 1; j < num_train_images; j++) {
            if (distances[j] < distances[i]) {
                double tmp_dist = distances[i];
                distances[i] = distances[j];
                distances[j] = tmp_dist;

                int tmp_label = labels[i];
                labels[i] = labels[j];
                labels[j] = tmp_label;
            }
        }
    }

    int count[10] = {0};
    for (int i = 0; i < k; i++) {
        count[labels[i]]++;
    }

    int max_count = 0, predicted_label = -1;
    for (int i = 0; i < 10; i++) {
        if (count[i] > max_count) {
            max_count = count[i];
            predicted_label = i;
        }
    }

    free(distances);
    free(labels);
    return predicted_label;
}

void free_images(Image *images, int num_images) {
    for (int i = 0; i < num_images; i++) {
        free(images[i].data);
    }
    free(images);
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Usage: %s <train-images> <train-labels> <test-images> <test-labels> <k>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *train_img_file = argv[1];
    const char *train_lbl_file = argv[2];
    const char *test_img_file  = argv[3];
    const char *test_lbl_file  = argv[4];
    int k = atoi(argv[5]);

    if (k <= 0) {
        fprintf(stderr, "Invalid value for k: %d\n", k);
        return EXIT_FAILURE;
    }

    Image *train_images, *test_images;
    int num_train, num_test;

    printf("📥 Loading training data...\n");
    if (read_mnist(train_img_file, train_lbl_file, &train_images, &num_train) != 0) {
        return EXIT_FAILURE;
    }

    printf("📥 Loading test data...\n");
    if (read_mnist(test_img_file, test_lbl_file, &test_images, &num_test) != 0) {
        free_images(train_images, num_train);
        return EXIT_FAILURE;
    }

    printf("🔍 Classifying %d test images using k=%d...\n", num_test, k);

    int correct = 0;
    for (int i = 0; i < num_test; i++) {
        int pred = classify_image(train_images, num_train, test_images[i].data, k);
        if (pred == test_images[i].label) correct++;
        if (i % 100 == 0 || i == num_test - 1) {
            printf("Test image %5d — Predicted: %d | Actual: %d\n", i, pred, test_images[i].label);
        }
    }

    double accuracy = 100.0 * correct / num_test;
    printf("\n✅ Accuracy: %.2f%% (%d/%d correct)\n", accuracy, correct, num_test);

    free_images(train_images, num_train);
    free_images(test_images, num_test);
    return EXIT_SUCCESS;
}
// TODO: Timing info per image or total run
// TODO: Output results to CSV or JSON
// TODO: Confusion matrix output
// TODO: Distance metric options (cosine, manhattan, etc.)
