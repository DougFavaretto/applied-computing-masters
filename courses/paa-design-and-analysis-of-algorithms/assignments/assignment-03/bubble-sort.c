#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void helper() {
    printf("Usage: bubblesort [--file FILE]\n");
    printf("Options:\n");
    printf("  --file FILE   Path to the input file containing integers (one per line)\n");
}

long long bubbleSort(int *v, size_t n) {
    long long operations = 0;
    for (size_t i = n; i > 1; i--) {
        int swapped = 0;
        for (size_t j = 0; j < i - 1; j++) {
            if (v[j] > v[j + 1]) {
                operations++;
                int aux = v[j];
                v[j] = v[j + 1];
                v[j + 1] = aux;
                swapped = 1;
            }
        }
        if (!swapped) {
            break;
        }
    }
    return operations;
}

int main (int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "--file") != 0) {
        fprintf(stderr, "Invalid arguments.\n");
        helper();
        return 1;
    }
    const char *filename = argv[2];
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Could not open file: %s\n", filename);
        return 1;
    }
    int *data = NULL;
    size_t size = 0, capacity = 0;
    int value;
    while (fscanf(f, "%d", &value) == 1) {
        if (size >= capacity) {
            capacity = capacity ? capacity * 2 : 1;
            int *new_data = realloc(data, capacity * sizeof(int));
            if (!new_data) {
                fprintf(stderr, "Memory allocation failed.\n");
                free(data);
                fclose(f);
                return 1;
            }
            data = new_data;
        }
        data[size] = value;
        size++;
    }
    fclose(f);
    if (size == 0) {
        fprintf(stderr, "No data to sort.\n");
        free(data);
        return 1;
    }
    long long operations = bubbleSort(data, size);
    printf("Number of operations: %lld\n", operations);
    free(data);
    return 0;
}