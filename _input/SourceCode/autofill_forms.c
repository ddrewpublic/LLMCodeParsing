// File: autofill_forms.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TEMPLATES 10
#define MAX_FIELDS 10
#define MAX_FIELD_SIZE 100

typedef struct {
    char fieldName[MAX_FIELD_SIZE];
    char fieldValue[MAX_FIELD_SIZE];
} Field;

typedef struct {
    char templateName[MAX_FIELD_SIZE];
    Field fields[MAX_FIELDS];
    int fieldCount;
} Template;

Template templates[MAX_TEMPLATES];
int templateCount = 0;

// Function to add a new template
void addTemplate(const char *name) {
    if (templateCount >= MAX_TEMPLATES) {
        printf("Maximum template limit reached.\n");
        return;
    }
    strcpy(templates[templateCount].templateName, name);
    templates[templateCount].fieldCount = 0;
    templateCount++;
}

// Function to add a field to a template
void addFieldToTemplate(const char *templateName, const char *fieldName, const char *fieldValue) {
    for (int i = 0; i < templateCount; i++) {
        if (strcmp(templates[i].templateName, templateName) == 0) {
            if (templates[i].fieldCount >= MAX_FIELDS) {
                printf("Maximum fields limit reached for template: %s\n", templateName);
                return;
            }
            strcpy(templates[i].fields[templates[i].fieldCount].fieldName, fieldName);
            strcpy(templates[i].fields[templates[i].fieldCount].fieldValue, fieldValue);
            templates[i].fieldCount++;
            return;
        }
    }
    printf("Template %s not found.\n", templateName);
}

// Function to display a template
void displayTemplate(const char *templateName) {
    for (int i = 0; i < templateCount; i++) {
        if (strcmp(templates[i].templateName, templateName) == 0) {
            printf("Template: %s\n", templates[i].templateName);
            for (int j = 0; j < templates[i].fieldCount; j++) {
                printf("  %s: %s\n", templates[i].fields[j].fieldName, templates[i].fields[j].fieldValue);
            }
            return;
        }
    }
    printf("Template %s not found.\n", templateName);
}

// Function to fill a form using a template
void fillForm(const char *templateName) {
    for (int i = 0; i < templateCount; i++) {
        if (strcmp(templates[i].templateName, templateName) == 0) {
            printf("Filling form using template: %s\n", templates[i].templateName);
            for (int j = 0; j < templates[i].fieldCount; j++) {
                printf("Field: %s | Value: %s\n", templates[i].fields[j].fieldName, templates[i].fields[j].fieldValue);
            }
            return;
        }
    }
    printf("Template %s not found.\n", templateName);
}

int main() {
    // Adding templates and fields
    addTemplate("Job Application");
    addFieldToTemplate("Job Application", "Name", "John Doe");
    addFieldToTemplate("Job Application", "Email", "john.doe@example.com");
    addFieldToTemplate("Job Application", "Phone", "123-456-7890");

    addTemplate("Feedback Form");
    addFieldToTemplate("Feedback Form", "Name", "Jane Smith");
    addFieldToTemplate("Feedback Form", "Comments", "Great service!");

    // Displaying templates
    displayTemplate("Job Application");
    displayTemplate("Feedback Form");

    // Filling forms
    fillForm("Job Application");
    fillForm("Feedback Form");

    return 0;
}
