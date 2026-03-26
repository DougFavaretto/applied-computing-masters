#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


void helper() {
    printf("This program generates datasets for sorting benchmarks.\n");
    printf("Usage: generate_data [--size N] [--seed S]\n");
    printf("  --size N   Number of integers to generate (default: 100000)\n");
    printf("  --seed S   Seed for random number generator (default: current time)\n");
}

void create_dataset(const char *filename, int *data, int size, int seed, int is_sorted_flag) {
    if (is_sorted_flag == 1) {
        for (int i = 0; i < size; i++)
            data[i] = i;
    } else if (is_sorted_flag == 2) {
        for (int i = 0; i < size; i++)
            data[i] = size - 1 - i;
    }
    else {
        for (int i = 0; i < size; i++)
            data[i] = i;

        srand(seed ? seed : time(NULL));
        for (int i = size - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            int temp = data[i];
            data[i] = data[j];
            data[j] = temp;
        }
    }

    /* Write to file */
    FILE *f = fopen(filename, "w");
    if (f) {
        for (int i = 0; i < size; i++) {
            fprintf(f, "%d\n", data[i]);
        }
        fclose(f);
        printf("Generated and saved dataset to: %s\n", filename);
    }
}

int main (int argc, char *argv[]) {
    size_t size = 100000;
    int seed = 0;
    char *end;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            helper();
            return 0;
        }
        else if (strcmp(argv[i], "--size") == 0 && i + 1 < argc) {
            size = strtol(argv[++i], &end, 10);
        }
        else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            seed = strtol(argv[++i], &end, 10);
        }
        else {
            fprintf(stderr, "Unknown or incomplete argument: %s\n", argv[i]);
            helper();
            return 1;
        }
    }

    int *random_data = malloc(size * sizeof(size_t));
    int *sorted_asc_data = malloc(size * sizeof(size_t));
    int *sorted_desc_data = malloc(size * sizeof(size_t));

    create_dataset("test_random_data.txt", random_data, size, seed, 0);
    create_dataset("test_sorted_asc_data.txt", sorted_asc_data, size, seed, 1);
    create_dataset("test_sorted_desc_data.txt", sorted_desc_data, size, seed, 2);

    free(random_data);
    free(sorted_asc_data);
    free(sorted_desc_data);
}