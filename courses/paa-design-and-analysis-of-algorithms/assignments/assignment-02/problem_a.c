#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

typedef struct {
    char algorithm[32];
    char dataset[32];
    int size;
    int runs;
    double median_seconds;
} BenchmarkResult;

void print_usage(const char *program_name) {
    printf("Usage: %s [--size N] [--runs R] [--output-prefix PREFIX]\n", program_name);
    printf("\nOptions:\n");
    printf("  --size N           Input size (default: 100000)\n");
    printf("  --runs R           Repetitions per algorithm/dataset pair (default: 3)\n");
    printf("  --output-prefix P  Output prefix for CSV file (default: results_a)\n");
    printf("  --help             Show this help message\n");
}

/* Sorting algorithms */

void bubbleSort(int *v, int n) {
    for (int i = n - 1; i > 0; i--) {
        for (int j = 0; j < i; j++) {
            if (v[j] > v[j + 1]) {
                int aux = v[j];
                v[j] = v[j + 1];
                v[j + 1] = aux;
            }
        }
    }
}

void insertionSort(int *v, int n) {
    for (int i = 1; i < n; i++) {
        int x = v[i];
        int j = i - 1;
        while (j >= 0 && v[j] > x) {
            v[j + 1] = v[j];
            j--;
        }
        v[j + 1] = x;
    }
}

void selectionSort(int *v, int n) {
    for (int i = 0; i < n; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            if (v[j] < v[min_idx])
                min_idx = j;
        }
        int aux = v[i];
        v[i] = v[min_idx];
        v[min_idx] = aux;
    }
}

/* Utility functions */

double get_time_seconds(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

int is_sorted(int *v, int n) {
    for (int i = 0; i < n - 1; i++) {
        if (v[i] > v[i + 1])
            return 0;
    }
    return 1;
}

double timed_run(void (*sort_func)(int *, int), int *dataset, int size) {
    int *sample = malloc(size * sizeof(int));
    if (!sample) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    memcpy(sample, dataset, size * sizeof(int));

    double start = get_time_seconds();
    sort_func(sample, size);
    double elapsed = get_time_seconds() - start;

    if (!is_sorted(sample, size)) {
        fprintf(stderr, "Sorting validation failed: output is not sorted\n");
        exit(1);
    }

    free(sample);
    return elapsed;
}

int compare_doubles(const void *a, const void *b) {
    double diff = *(double *)a - *(double *)b;
    if (diff < 0) return -1;
    if (diff > 0) return 1;
    return 0;
}

double median(double *arr, int n) {
    qsort(arr, n, sizeof(double), compare_doubles);
    return arr[n / 2];
}

void load_or_create_dataset(const char *filename, int *data, int size, int is_sorted_flag) {
    FILE *f = fopen(filename, "r");
    if (f) {
        int count = 0;
        while (count < size && fscanf(f, "%d", &data[count]) == 1) {
            count++;
        }
        fclose(f);
        if (count == size) {
            printf("Loaded dataset from: %s\n", filename);
            return;
        }
    }

    /* Generate dataset */
    if (is_sorted_flag) {
        for (int i = 0; i < size; i++)
            data[i] = i;
    } else {
        srand(42);
        for (int i = 0; i < size; i++)
            data[i] = rand() % 1000000000;
    }

    /* Write to file */
    f = fopen(filename, "w");
    if (f) {
        for (int i = 0; i < size; i++) {
            fprintf(f, "%d\n", data[i]);
        }
        fclose(f);
        printf("Generated and saved dataset to: %s\n", filename);
    }
}

int main(int argc, char *argv[]) {
    int size = 100000;
    int runs = 3;
    const char *output_prefix = "results_a";

    /* Parse arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "--size") == 0 && i + 1 < argc) {
            size = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--runs") == 0 && i + 1 < argc) {
            runs = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--output-prefix") == 0 && i + 1 < argc) {
            output_prefix = argv[++i];
        } else {
            fprintf(stderr, "Unknown or incomplete argument: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }

    if (size <= 0 || runs <= 0) {
        fprintf(stderr, "Error: --size and --runs must be positive integers\n");
        print_usage(argv[0]);
        return 1;
    }

    printf("Benchmark Parameters: n=%d, runs=%d\n\n", size, runs);

    /* Allocate memory for datasets */
    int *random_data = malloc(size * sizeof(int));
    int *sorted_data = malloc(size * sizeof(int));
    if (!random_data || !sorted_data) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    /* Load or create datasets */
    char random_file[256], sorted_file[256];
    snprintf(random_file, sizeof(random_file), "input_random_%d.txt", size);
    snprintf(sorted_file, sizeof(sorted_file), "input_sorted_%d.txt", size);

    load_or_create_dataset(random_file, random_data, size, 0);
    load_or_create_dataset(sorted_file, sorted_data, size, 1);

    printf("\nRunning benchmark with n=%d, runs=%d...\n\n", size, runs);

    /* Define algorithms */
    typedef struct {
        const char *name;
        void (*func)(int *, int);
    } Algorithm;

    Algorithm algorithms[] = {
        {"Bubble Sort", bubbleSort},
        {"Insertion Sort", insertionSort},
        {"Selection Sort", selectionSort}
    };

    typedef struct {
        const char *name;
        int *data;
    } Dataset;

    Dataset datasets[] = {
        {"Random", random_data},
        {"Already Sorted", sorted_data}
    };

    int num_algorithms = sizeof(algorithms) / sizeof(algorithms[0]);
    int num_datasets = sizeof(datasets) / sizeof(datasets[0]);
    int total_results = num_algorithms * num_datasets;

    BenchmarkResult *results = malloc(total_results * sizeof(BenchmarkResult));
    if (!results) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    int result_idx = 0;

    /* Run benchmarks */
    for (int a = 0; a < num_algorithms; a++) {
        for (int d = 0; d < num_datasets; d++) {
            printf("Running %s on %s...\n", algorithms[a].name, datasets[d].name);

            double *measurements = malloc(runs * sizeof(double));
            if (!measurements) {
                fprintf(stderr, "Memory allocation failed\n");
                return 1;
            }

            for (int r = 0; r < runs; r++) {
                measurements[r] = timed_run(algorithms[a].func, datasets[d].data, size);
            }

            BenchmarkResult *res = &results[result_idx++];
            strcpy(res->algorithm, algorithms[a].name);
            strcpy(res->dataset, datasets[d].name);
            res->size = size;
            res->runs = runs;
            res->median_seconds = median(measurements, runs);

            printf("  Median time: %.6f seconds\n", res->median_seconds);

            free(measurements);
        }
    }

    /* Write CSV */
    char csv_path[512];
    snprintf(csv_path, sizeof(csv_path), "%s.csv", output_prefix);
    FILE *csv_file = fopen(csv_path, "w");
    if (csv_file) {
        fprintf(csv_file, "algorithm,dataset,size,runs,median_seconds\n");
        for (int i = 0; i < total_results; i++) {
            fprintf(csv_file, "%s,%s,%d,%d,%.6f\n",
                    results[i].algorithm,
                    results[i].dataset,
                    results[i].size,
                    results[i].runs,
                    results[i].median_seconds);
        }
        fclose(csv_file);
        printf("\nCSV report: %s\n", csv_path);
    }

    printf("\nBenchmark completed.\n");

    /* Cleanup */
    free(random_data);
    free(sorted_data);
    free(results);

    return 0;
}
