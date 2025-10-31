// File: roman_converter.c

#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Function to convert a Roman numeral to an integer
int romanToInt(const char *s) {
    int total = 0;
    int prevValue = 0;
    int currentValue = 0;

    // Mapping of Roman numerals to their integer values
    for (int i = strlen(s) - 1; i >= 0; i--) {
        switch (s[i]) {
            case 'I': currentValue = 1; break;
            case 'V': currentValue = 5; break;
            case 'X': currentValue = 10; break;
            case 'L': currentValue = 50; break;
            case 'C': currentValue = 100; break;
            case 'D': currentValue = 500; break;
            case 'M': currentValue = 1000; break;
            default: return -1; // Invalid character
        }

        // If the current value is less than the previous value, subtract it
        if (currentValue < prevValue) {
            total -= currentValue;
        } else {
            total += currentValue;
        }
        prevValue = currentValue;
    }
    return total;
}

// Function to convert an integer to a Roman numeral
void intToRoman(int num, char *result) {
    const struct {
        int value;
        const char *numeral;
    } romanNumerals[] = {
        {1000, "M"}, {900, "CM"}, {500, "D"}, {400, "CD"},
        {100, "C"}, {90, "XC"}, {50, "L"}, {40, "XL"},
        {10, "X"}, {9, "IX"}, {5, "V"}, {4, "IV"},
        {1, "I"}
    };

    result[0] = '\0'; // Initialize result string
    for (int i = 0; i < sizeof(romanNumerals) / sizeof(romanNumerals[0]); i++) {
        while (num >= romanNumerals[i].value) {
            strcat(result, romanNumerals[i].numeral);
            num -= romanNumerals[i].value;
        }
    }
}

// Main function to interact with the user
int main() {
    char choice;
    printf("Choose conversion:\n");
    printf("1: Roman to Integer\n");
    printf("2: Integer to Roman\n");
    printf("Enter your choice (1 or 2): ");
    scanf(" %c", &choice);

    if (choice == '1') {
        char roman[20];
        printf("Enter a Roman numeral: ");
        scanf("%s", roman);
        // Convert to uppercase for consistency
        for (int i = 0; roman[i]; i++) {
            roman[i] = toupper(roman[i]);
        }
        int result = romanToInt(roman);
        if (result != -1) {
            printf("The integer value is: %d\n", result);
        } else {
            printf("Invalid Roman numeral.\n");
        }
    } else if (choice == '2') {
        int number;
        char result[20];
        printf("Enter an integer (1-3999): ");
        scanf("%d", &number);
        if (number < 1 || number > 3999) {
            printf("Please enter a number between 1 and 3999.\n");
        } else {
            intToRoman(number, result);
            printf("The Roman numeral is: %s\n", result);
        }
    } else {
        printf("Invalid choice.\n");
    }

    return 0;
}
