// File: house_price_predictor.c

#include <stdio.h>
#include <stdlib.h>

#define DATA_SIZE 5 // Number of data points

// Structure to hold data points
typedef struct {
    double size; // Size of the house (in square feet)
    double price; // Price of the house
} HouseData;

// Function to calculate the mean
double calculate_mean(double *data, int size) {
    double sum = 0.0;
    for (int i = 0; i < size; i++) {
        sum += data[i];
    }
    return sum / size;
}

// Function to calculate the slope (m) and intercept (b) of the linear regression line
void linear_regression(HouseData *data, int size, double *m, double *b) {
    double x_mean = calculate_mean((double *)data, size);
    double y_mean = calculate_mean((double *)&data->price, size);
    
    double numerator = 0.0;
    double denominator = 0.0;

    for (int i = 0; i < size; i++) {
        numerator += (data[i].size - x_mean) * (data[i].price - y_mean);
        denominator += (data[i].size - x_mean) * (data[i].size - x_mean);
    }

    *m = numerator / denominator; // Slope
    *b = y_mean - (*m * x_mean); // Intercept
}

// Function to predict the price based on size
double predict_price(double size, double m, double b) {
    return (m * size) + b;
}

int main() {
    // Sample dataset: size in square feet and corresponding prices
    HouseData data[DATA_SIZE] = {
        {1500, 300000},
        {1600, 320000},
        {1700, 340000},
        {1800, 360000},
        {1900, 380000}
    };

    double m, b;
    
    // Perform linear regression on the dataset
    linear_regression(data, DATA_SIZE, &m, &b);

    printf("Linear Regression Equation: Price = %.2fx + %.2f\n", m, b);

    // Example prediction
    double size_to_predict;
    printf("Enter the size of the house (in square feet) to predict its price: ");
    scanf("%lf", &size_to_predict);

    double predicted_price = predict_price(size_to_predict, m, b);
    printf("Predicted price for a house of size %.2f square feet: $%.2f\n", size_to_predict, predicted_price);

    return 0;
}
