// File: contact_sort_deduplicate.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CONTACTS 1000
#define MAX_NAME_LENGTH 100
#define MAX_PHONE_LENGTH 15

typedef struct {
    char name[MAX_NAME_LENGTH];
    char phone[MAX_PHONE_LENGTH];
} Contact;

// Function prototypes
int load_contacts(const char *filename, Contact contacts[], int max_contacts);
int compare_contacts(const void *a, const void *b);
void deduplicate_contacts(Contact contacts[], int *count);
void print_contacts(const Contact contacts[], int count);

int main() {
    Contact contacts[MAX_CONTACTS];
    int count = load_contacts("contacts.txt", contacts, MAX_CONTACTS);
    
    if (count == -1) {
        fprintf(stderr, "Error loading contacts.\n");
        return 1;
    }

    // Sort contacts by name
    qsort(contacts, count, sizeof(Contact), compare_contacts);

    // Deduplicate contacts
    deduplicate_contacts(contacts, &count);

    // Print sorted and deduplicated contacts
    print_contacts(contacts, count);

    return 0;
}

// Load contacts from a file
int load_contacts(const char *filename, Contact contacts[], int max_contacts) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return -1;
    }

    int count = 0;
    while (count < max_contacts && fscanf(file, "%99[^,],%14s\n", contacts[count].name, contacts[count].phone) == 2) {
        count++;
    }

    fclose(file);
    return count;
}

// Compare function for qsort
int compare_contacts(const void *a, const void *b) {
    return strcmp(((Contact *)a)->name, ((Contact *)b)->name);
}

// Remove duplicates from the contacts array
void deduplicate_contacts(Contact contacts[], int *count) {
    int unique_count = 0;

    for (int i = 0; i < *count; i++) {
        if (i == 0 || strcmp(contacts[i].name, contacts[unique_count - 1].name) != 0) {
            contacts[unique_count++] = contacts[i];
        }
    }

    *count = unique_count;
}

// Print the contacts
void print_contacts(const Contact contacts[], int count) {
    printf("Sorted and Deduplicated Contacts:\n");
    for (int i = 0; i < count; i++) {
        printf("%s, %s\n", contacts[i].name, contacts[i].phone);
    }
}
