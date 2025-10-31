// File: stock_monitor.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_STOCKS 100
#define MAX_NAME_LEN 50
#define MAX_DATA_POINTS 100

typedef struct {
    char name[MAX_NAME_LEN];
    double prices[MAX_DATA_POINTS];
    int count;
} Stock;

void add_stock(Stock *stocks, int *stock_count, const char *name) {
    if (*stock_count < MAX_STOCKS) {
        strncpy(stocks[*stock_count].name, name, MAX_NAME_LEN);
        stocks[*stock_count].count = 0;
        (*stock_count)++;
    } else {
        printf("Maximum number of stocks reached.\n");
    }
}

void add_price(Stock *stock, double price) {
    if (stock->count < MAX_DATA_POINTS) {
        stock->prices[stock->count] = price;
        stock->count++;
    } else {
        printf("Maximum data points reached for %s.\n", stock->name);
    }
}

void print_stock_trends(Stock *stocks, int stock_count) {
    for (int i = 0; i < stock_count; i++) {
        printf("Stock: %s\n", stocks[i].name);
        for (int j = 0; j < stocks[i].count; j++) {
            printf("%.2f ", stocks[i].prices[j]);
        }
        printf("\n");
    }
}

void simulate_price_updates(Stock *stocks, int stock_count) {
    for (int i = 0; i < stock_count; i++) {
        // Simulate a price update (random price for demonstration)
        double new_price = (rand() % 10000) / 100.0;
        add_price(&stocks[i], new_price);
    }
}

int main() {
    Stock stocks[MAX_STOCKS];
    int stock_count = 0;

    // Add some stocks to monitor
    add_stock(stocks, &stock_count, "AAPL");
    add_stock(stocks, &stock_count, "GOOGL");
    add_stock(stocks, &stock_count, "AMZN");

    // Simulate monitoring stock prices
    for (int i = 0; i < 10; i++) { // Simulate 10 price updates
        simulate_price_updates(stocks, stock_count);
        print_stock_trends(stocks, stock_count);
        sleep(1); // Wait for a second before the next update
    }

    return 0;
}
