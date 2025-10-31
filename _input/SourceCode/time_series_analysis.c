// File: time_series_analysis.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_DATA_POINTS 1000

// Function prototypes
void readData(const char *filename, double *data, int *size);
void calculateTrends(double *data, int size, double *slope, double *intercept);
void printData(double *data, int size);
void plotData(double *data, int size, double slope, double intercept);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <data_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    double data[MAX_DATA_POINTS];
    int size = 0;

    // Read data from the specified file
    readData(argv[1], data, &size);

    // Print the raw data
    printf("Raw Data:\n");
    printData(data, size);

    // Calculate the trend line (slope and intercept)
    double slope, intercept;
    calculateTrends(data, size, &slope, &intercept);
    printf("Trend line: y = %.2fx + %.2f\n", slope, intercept);

    // Plot the data with the trend line
    plotData(data, size, slope, intercept);

    return EXIT_SUCCESS;
}

// Function to read data from a file
void readData(const char *filename, double *data, int *size) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    while (fscanf(file, "%lf", &data[*size]) == 1 && *size < MAX_DATA_POINTS) {
        (*size)++;
    }

    fclose(file);
}

// Function to calculate the slope and intercept of the trend line
void calculateTrends(double *data, int size, double *slope, double *intercept) {
    double sum_x = 0.0, sum_y = 0.0, sum_xy = 0.0, sum_x2 = 0.0;

    for (int i = 0; i < size; i++) {
        sum_x += i;          // x values are indices
        sum_y += data[i];   // y values are the data points
        sum_xy += i * data[i];
        sum_x2 += i * i;
    }

    *slope = (size * sum_xy - sum_x * sum_y) / (size * sum_x2 - sum_x * sum_x);
    *intercept = (sum_y - (*slope) * sum_x) / size;
}

// Function to print the data
void printData(double *data, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d: %.2f\n", i, data[i]);
    }
}

// Function to plot data and trend line (simple ASCII representation)
void plotData(double *data, int size, double slope, double intercept) {
    printf("\nPlotting Data:\n");

    for (int i = 0; i < size; i++) {
        double trend_value = slope * i + intercept;
        printf("%3d | ", i);
        for (int j = 0; j < (int)(data[i] + 0.5); j++) {
            printf("*");
        }
        printf(" (Data) | ");
        for (int j = 0; j < (int)(trend_value + 0.5); j++) {
            printf("-");
        }
        printf(" (Trend)\n");
    }
}
