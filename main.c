#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FlexAlloc.h"

// Constants for initial settings
#define INITIAL_CAPACITY 4
#define ALIGNMENT 16
#define COMMAND_SIZE 10

// Dynamic array structure
typedef struct {
    int* data;
    size_t size;
    size_t capacity;
    size_t alignment;
} DynamicArray;

// Initialize the dynamic array
static void initArray(DynamicArray* array, size_t initial_capacity, size_t alignment) {
    array->data = NULL;
    array->size = 0;
    array->capacity = initial_capacity;
    array->alignment = alignment;
    memory_block block = allocate(initial_capacity * sizeof(int), alignment);
    if (block.ptr != NULL) {
        array->data = (int*)block.ptr;
        memset(array->data, 0, initial_capacity * sizeof(int)); // Initialize memory
    }
    else {
        printf("Failed to allocate initial memory.\n");
    }
}

// Free the dynamic array
static void freeArray(DynamicArray* array) {
    if (array->data != NULL) {
        deallocate((memory_block) { array->data, array->capacity * sizeof(int) }, array->alignment);
        array->data = NULL;
    }
    array->size = 0;
    array->capacity = 0;
}

// Resize the dynamic array
static int resizeArray(DynamicArray* array, size_t new_capacity) {
    if (new_capacity > array->capacity) {
        memory_block block = { array->data, array->capacity * sizeof(int) };
        if (try_expand(&block, new_capacity * sizeof(int))) {
            array->data = (int*)block.ptr;
            array->capacity = new_capacity;
        }
        else {
            memory_block new_block = allocate(new_capacity * sizeof(int), array->alignment);
            if (new_block.ptr != NULL) {
                memcpy(new_block.ptr, array->data, array->size * sizeof(int));
                deallocate((memory_block) { array->data, array->capacity * sizeof(int) }, array->alignment);
                array->data = (int*)new_block.ptr;
                array->capacity = new_capacity;
            }
            else {
                printf("Failed to allocate memory during resize.\n");
                return 0; // Failure
            }
        }
    }
    return 1; // Success
}

// Add an element to the dynamic array
static int addElement(DynamicArray* array, int element) {
    if (array->size == array->capacity) {
        size_t new_capacity = array->capacity * 2;
        if (!resizeArray(array, new_capacity)) {
            return 0; // Failure
        }
    }
    array->data[array->size] = element;
    array->size++;
    return 1; // Success
}

// Print the dynamic array
static void printArray(const DynamicArray* array) {
    printf("Array size: %zu, capacity: %zu\n", array->size, array->capacity);
    for (size_t i = 0; i < array->size; i++) {
        printf("%d ", array->data[i]);
    }
    printf("\n");
}

// Safe string comparison function
static int safe_strcmp(const char* s1, const char* s2) {
    if (s1 == NULL || s2 == NULL) {
        return -1;
    }
    return strcmp(s1, s2);
}

// Process user command
static void processCommand(DynamicArray* array, const char* command, int value) {
    if (safe_strcmp(command, "add") == 0) {
        if (!addElement(array, value)) {
            printf("Failed to add element %d\n", value);
        }
    }
    else if (safe_strcmp(command, "print") == 0) {
        printArray(array);
    }
    else if (safe_strcmp(command, "exit") == 0) {
        // Do nothing, the main loop will handle exiting
    }
    else {
        printf("Unknown command. Please try again.\n");
    }
}

int main() {
    DynamicArray array;
    initArray(&array, INITIAL_CAPACITY, ALIGNMENT);

    char command[COMMAND_SIZE];
    int value;

    while (1) {
        printf("Enter 'add <value>' to add an element, 'print' to display the array, or 'exit' to quit: ");
        if (scanf("%9s", command) != 1) {
            continue;
        }

        if (safe_strcmp(command, "exit") == 0) {
            break;
        }

        if (safe_strcmp(command, "add") == 0) {
            if (scanf("%d", &value) != 1) {
                printf("Invalid value. Please enter an integer.\n");
                // Clear the input buffer
                int c;
                while ((c = getchar()) != '\n' && c != EOF);
                continue;
            }
        }
        else {
            value = 0; // No value needed for other commands
        }

        processCommand(&array, command, value);
    }

    freeArray(&array);

    return 0;
}
