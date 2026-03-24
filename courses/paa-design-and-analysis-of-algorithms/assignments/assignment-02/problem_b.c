#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

double get_time_seconds(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

int compare_doubles(const void *a, const void *b) {
    return (*(double *)a > *(double *)b) - (*(double *)a < *(double *)b);
}

double median(double *arr, int n) {
    qsort(arr, n, sizeof(double), compare_doubles);
    return arr[n / 2];
}

long long power_calculate(long long a, int b) {
    if (b == 0) return 1;
    if (b == 1) return a;
    long long half = power_calculate(a, b / 2);
    return (b % 2) ? half * half * a : half * half;
}

void print_help(const char *program) {
    printf("Usage:\n");
    printf("  %s <power|matrix> [options]\n\n", program);
    printf("Modes:\n");
    printf("  power   Benchmark exponentiation (a^b)\n");
    printf("  matrix  Benchmark n x n matrix multiplication\n\n");
    printf("Global options:\n");
    printf("  -h, --help            Show this help message\n\n");
    printf("Power options:\n");
    printf("  --base A              Base value (default: 2)\n");
    printf("  --exp B               Exponent value (default: 30)\n");
    printf("  --runs R              Number of runs (default: 3)\n");
    printf("  --output-prefix NAME  Output CSV prefix (default: results_b_power)\n\n");
    printf("Matrix options:\n");
    printf("  --size N              Matrix order n (default: 300)\n");
    printf("  --runs R              Number of runs (default: 3)\n");
    printf("  --output-prefix NAME  Output CSV prefix (default: results_b_matrix)\n");
}

int is_help_flag(const char *arg) {
    return !strcmp(arg, "-h") || !strcmp(arg, "--help");
}

int ensure_option_value(const char *program, const char *option, int i, int argc) {
    if (i + 1 < argc) return 1;
    fprintf(stderr, "Missing value for option %s\n", option);
    fprintf(stderr, "Use %s --help to see available options.\n", program);
    return 0;
}

int run_power(int argc, char *argv[]) {
    long long base = 2;
    int exp = 30, runs = 3;
    const char *output = "results_b_power";

    for (int i = 2; i < argc; i++) {
        if (is_help_flag(argv[i])) {
            print_help(argv[0]);
            return 0;
        } else if (!strcmp(argv[i], "--base")) {
            if (!ensure_option_value(argv[0], "--base", i, argc)) return 1;
            base = atoll(argv[++i]);
        } else if (!strcmp(argv[i], "--exp")) {
            if (!ensure_option_value(argv[0], "--exp", i, argc)) return 1;
            exp = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "--runs")) {
            if (!ensure_option_value(argv[0], "--runs", i, argc)) return 1;
            runs = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "--output-prefix")) {
            if (!ensure_option_value(argv[0], "--output-prefix", i, argc)) return 1;
            output = argv[++i];
        } else {
            fprintf(stderr, "Unknown option for power mode: %s\n", argv[i]);
            fprintf(stderr, "Use %s --help to see available options.\n", argv[0]);
            return 1;
        }
    }

    if (runs <= 0) {
        fprintf(stderr, "--runs must be greater than 0\n");
        return 1;
    }

    printf("Power Benchmark: base=%lld, exp=%d, runs=%d\n\n", base, exp, runs);
    double *m1 = malloc(runs * sizeof(double));
    double *m2 = malloc(runs * sizeof(double));
    if (!m1 || !m2) {
        fprintf(stderr, "Failed to allocate memory for power benchmark.\n");
        free(m1);
        free(m2);
        return 1;
    }

    printf("Running power_calculate...\n");
    double *m = malloc(runs * sizeof(double));
    if (!m) {
        fprintf(stderr, "Failed to allocate memory for power benchmark.\n");
        return 1;
    }
    for (int r = 0; r < runs; r++) {
        double start = get_time_seconds();
        long long res = power_calculate(base, exp);
        m[r] = get_time_seconds() - start;
        printf("  Run %d: %.9f seconds (result=%lld)\n", r + 1, m[r], res);
    }
    double med = median(m, runs);
    printf("  Median: %.9f seconds\n\n", med);

    char path[256];
    snprintf(path, sizeof(path), "%s.csv", output);
    FILE *f = fopen(path, "w");
    if (f) {
        fprintf(f, "algorithm,base,exponent,runs,median_seconds\n");
        fprintf(f, "power_calculate,%lld,%d,%d,%.9f\n", base, exp, runs, med);
        fclose(f);
        printf("CSV report: %s\n", path);
    }
    free(m);
    printf("Power benchmark completed.\n");
    return 0;
}

int run_matrix(int argc, char *argv[]) {
    int size = 300, runs = 3;
    const char *output = "results_b_matrix";

    for (int i = 2; i < argc; i++) {
        if (is_help_flag(argv[i])) {
            print_help(argv[0]);
            return 0;
        } else if (!strcmp(argv[i], "--size")) {
            if (!ensure_option_value(argv[0], "--size", i, argc)) return 1;
            size = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "--runs")) {
            if (!ensure_option_value(argv[0], "--runs", i, argc)) return 1;
            runs = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "--output-prefix")) {
            if (!ensure_option_value(argv[0], "--output-prefix", i, argc)) return 1;
            output = argv[++i];
        } else {
            fprintf(stderr, "Unknown option for matrix mode: %s\n", argv[i]);
            fprintf(stderr, "Use %s --help to see available options.\n", argv[0]);
            return 1;
        }
    }

    if (size <= 0) {
        fprintf(stderr, "--size must be greater than 0\n");
        return 1;
    }
    if (runs <= 0) {
        fprintf(stderr, "--runs must be greater than 0\n");
        return 1;
    }

    printf("Matrix Multiplication: n=%d, runs=%d\n\n", size, runs);
    int *a = malloc(size * size * sizeof(int));
    int *b = malloc(size * size * sizeof(int));
    if (!a || !b) {
        fprintf(stderr, "Failed to allocate input matrices.\n");
        free(a);
        free(b);
        return 1;
    }

    for (int i = 0; i < size * size; i++)
        a[i] = i + 1, b[i] = i + 1;

    printf("Running matrix multiplication...\n");
    double *m = malloc(runs * sizeof(double));
    if (!m) {
        fprintf(stderr, "Failed to allocate measurements array.\n");
        free(a);
        free(b);
        return 1;
    }

    for (int r = 0; r < runs; r++) {
        int *res = malloc(size * size * sizeof(int));
        if (!res) {
            fprintf(stderr, "Failed to allocate result matrix.\n");
            free(a);
            free(b);
            free(m);
            return 1;
        }
        double start = get_time_seconds();
        for (int i = 0; i < size; i++)
            for (int j = 0; j < size; j++) {
                res[i * size + j] = 0;
                for (int k = 0; k < size; k++)
                    res[i * size + j] += a[i * size + k] * b[k * size + j];
            }
        m[r] = get_time_seconds() - start;
        printf("  Run %d: %.6f seconds\n", r + 1, m[r]);
        free(res);
    }
    double med = median(m, runs);
    printf("  Median: %.6f seconds\n\n", med);

    char path[256];
    snprintf(path, sizeof(path), "%s.csv", output);
    FILE *f = fopen(path, "w");
    if (f) {
        fprintf(f, "operation,matrix_size,runs,median_seconds\n");
        fprintf(f, "matrix_multiply,%d,%d,%.6f\n", size, runs, med);
        fclose(f);
        printf("CSV report: %s\n", path);
    }
    free(a);
    free(b);
    free(m);
    printf("Matrix benchmark completed.\n");
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_help(argv[0]);
        return 1;
    }

    if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
        print_help(argv[0]);
        return 0;
    }

    if (strcmp(argv[1], "power") == 0) {
        return run_power(argc, argv);
    } else if (strcmp(argv[1], "matrix") == 0) {
        return run_matrix(argc, argv);
    } else {
        fprintf(stderr, "Unknown mode: %s\n", argv[1]);
        fprintf(stderr, "Use --help to see available options.\n");
        return 1;
    }
}
