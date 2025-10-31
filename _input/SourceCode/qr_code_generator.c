// File: qr_code_generator.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QR_SIZE 21 // Size of the QR code (21x21 for version 1)
#define QR_MODULE_SIZE 5 // Size of each module in pixels

// Function to generate a simple QR code pattern for a given text
void generate_qr_code(const char *text, char qr[QR_SIZE][QR_SIZE]) {
    // Initialize the QR code matrix with spaces
    for (int i = 0; i < QR_SIZE; i++) {
        for (int j = 0; j < QR_SIZE; j++) {
            qr[i][j] = ' ';
        }
    }

    // Simple encoding for demonstration purposes (not a real QR code)
    for (int i = 0; i < strlen(text); i++) {
        int row = (i % QR_SIZE);
        int col = (i / QR_SIZE) % QR_SIZE;
        qr[row][col] = (text[i] % 2) ? '#' : ' ';
    }

    // Add finder patterns (not functional QR code, just for visualization)
    for (int i = 0; i < 7; i++) {
        qr[0][i] = qr[i][0] = qr[QR_SIZE - 1][i] = qr[i][QR_SIZE - 1] = '#';
    }
}

// Function to print the QR code in a visual format
void print_qr_code(char qr[QR_SIZE][QR_SIZE]) {
    for (int i = 0; i < QR_SIZE; i++) {
        for (int j = 0; j < QR_SIZE; j++) {
            printf("%c ", qr[i][j]);
        }
        printf("\n");
    }
}

// Main function to take input and generate the QR code
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <text or URL>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char qr[QR_SIZE][QR_SIZE];
    generate_qr_code(argv[1], qr);
    print_qr_code(qr);

    return EXIT_SUCCESS;
}
