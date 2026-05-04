#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *filename;
    long long bubble_ops;
    long long heap_ops;
    long long radix_ops;
    long long bubble_writes;
    long long heap_writes;
    long long radix_writes;
} ResultRow;

static void print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s [--output FILE.html] INPUT1.txt [INPUT2.txt ...]\n", program_name);
}

static char *duplicate_string(const char *s) {
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) {
        return NULL;
    }
    memcpy(copy, s, len + 1);
    return copy;
}

static const char *file_basename(const char *path) {
    const char *slash = strrchr(path, '/');
    return slash ? slash + 1 : path;
}

static int is_sorted_non_decreasing(const int *v, size_t n) {
    for (size_t i = 1; i < n; i++) {
        if (v[i - 1] > v[i]) {
            return 0;
        }
    }
    return 1;
}

static int load_file_data(const char *filename, int **out_data, size_t *out_size) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Could not open file: %s\n", filename);
        return 0;
    }

    size_t size = 0;
    size_t capacity = 1024;
    int *data = malloc(capacity * sizeof(int));
    if (!data) {
        fclose(f);
        fprintf(stderr, "Memory allocation failed while reading %s\n", filename);
        return 0;
    }

    int value;
    while (fscanf(f, "%d", &value) == 1) {
        if (size == capacity) {
            capacity *= 2;
            int *new_data = realloc(data, capacity * sizeof(int));
            if (!new_data) {
                free(data);
                fclose(f);
                fprintf(stderr, "Memory reallocation failed while reading %s\n", filename);
                return 0;
            }
            data = new_data;
        }
        data[size++] = value;
    }

    fclose(f);

    if (size == 0) {
        fprintf(stderr, "Input file has no integer data: %s\n", filename);
        free(data);
        return 0;
    }

    *out_data = data;
    *out_size = size;
    return 1;
}

static int *copy_array(const int *src, size_t n) {
    int *dst = malloc(n * sizeof(int));
    if (!dst) {
        return NULL;
    }
    memcpy(dst, src, n * sizeof(int));
    return dst;
}

static inline void swap_and_count(int *a, int *b, long long *writes) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
    if (writes) {
        *writes += 2; /* two element writes */
    }
}

typedef long long (*sort_fn)(int *v, size_t n, long long *writes);

static int measure_and_validate(sort_fn fn, const int *data, size_t n, long long *ops_out, long long *writes_out) {
    int *tmp = copy_array(data, n);
    if (!tmp) return 0;
    *ops_out = fn(tmp, n, NULL);
    int ok = is_sorted_non_decreasing(tmp, n);
    free(tmp);

    tmp = copy_array(data, n);
    if (!tmp) return 0;
    long long w = 0;
    fn(tmp, n, &w);
    *writes_out = w;
    free(tmp);

    return ok;
}

static long long bubble_sort_count_comparisons(int *v, size_t n, long long *writes) {
    long long comparisons = 0;

    for (size_t end = n; end > 1; end--) {
        int swapped = 0;
        for (size_t j = 0; j < end - 1; j++) {
            comparisons++;
            if (v[j] > v[j + 1]) {
                swap_and_count(&v[j], &v[j + 1], writes);
                swapped = 1;
            }
        }
        if (!swapped) {
            break;
        }
    }

    return comparisons;
}

static long long heapify_count_comparisons(int *v, size_t n, size_t root, long long *writes) {
    long long comparisons = 0;
    size_t largest = root;

    while (1) {
        size_t left = 2 * largest + 1;
        size_t right = 2 * largest + 2;
        size_t candidate = largest;

        if (left < n) {
            comparisons++;
            if (v[left] > v[candidate]) {
                candidate = left;
            }
        }

        if (right < n) {
            comparisons++;
            if (v[right] > v[candidate]) {
                candidate = right;
            }
        }

        if (candidate == largest) {
            break;
        }

        swap_and_count(&v[largest], &v[candidate], writes);
        largest = candidate;
    }

    return comparisons;
}

static long long heap_sort_count_comparisons(int *v, size_t n, long long *writes) {
    long long comparisons = 0;

    if (n < 2) {
        return 0;
    }

    for (size_t i = n / 2; i > 0; i--) {
        comparisons += heapify_count_comparisons(v, n, i - 1, writes);
    }

    for (size_t i = n; i > 1; i--) {
        swap_and_count(&v[0], &v[i - 1], writes);
        comparisons += heapify_count_comparisons(v, i - 1, 0, writes);
    }

    return comparisons;
}

/* write-counting is now integrated into the primary routines via swap_and_count */
static void counting_sort_exp_with_writes(int *v, size_t n, int exp, long long *writes) {
    int count[10] = {0};
    int *output = malloc(n * sizeof(int));
    if (!output) {
        fprintf(stderr, "Memory allocation failed in radix sort\n");
        exit(1);
    }

    for (size_t i = 0; i < n; i++) {
        int digit = (v[i] / exp) % 10;
        count[digit]++;
    }

    for (int i = 1; i < 10; i++) {
        count[i] += count[i - 1];
    }

    for (size_t i = n; i > 0; i--) {
        int value = v[i - 1];
        int digit = (value / exp) % 10;
        output[count[digit] - 1] = value;
        count[digit]--;
        if (writes) *writes += 1;
    }

    for (size_t i = 0; i < n; i++) {
        v[i] = output[i];
        if (writes) *writes += 1;
    }

    free(output);
    return;
}

static long long radix_sort_count_comparisons(int *v, size_t n, long long *writes) {
    if (n < 2) {
        return 0;
    }

    /* radix still has zero element-to-element comparisons */
    int min = v[0];
    int max = v[0];
    for (size_t i = 1; i < n; i++) {
        if (v[i] < min) {
            min = v[i];
        }
        if (v[i] > max) {
            max = v[i];
        }
    }

    if (min < 0) {
        int offset = -min;
        for (size_t i = 0; i < n; i++) {
            v[i] += offset;
            if (writes) *writes += 1;
        }
        max += offset;
    }

    for (int exp = 1; max / exp > 0; exp *= 10) {
        counting_sort_exp_with_writes(v, n, exp, writes);
    }

    if (min < 0) {
        int offset = -min;
        for (size_t i = 0; i < n; i++) {
            v[i] -= offset;
            if (writes) *writes += 1;
        }
    }

    return 0;
}

/* radix sort comparison-count function removed (writes-aware variant is used) */
static int write_html_report(const char *output_path, const ResultRow *rows, size_t row_count) {
    FILE *f = fopen(output_path, "w");
    if (!f) {
        fprintf(stderr, "Could not create output file: %s\n", output_path);
        return 0;
    }

    fprintf(f, "<html>\n");
    fprintf(f, "    <head>\n");
    fprintf(f, "        <title>Assignment 05</title>\n");
    fprintf(f, "    </head>\n");
    fprintf(f, "    <body>\n");
    fprintf(f, "        <table border=1>\n");
    fprintf(f, "            <tr>\n");
    fprintf(f, "                <th>File</th>\n");
    fprintf(f, "                <th>BubbleSort</th>\n");
    fprintf(f, "                <th>Heapsort</th>\n");
    fprintf(f, "                <th>Radixsort*</th>\n");
    fprintf(f, "            </tr>\n");
    for (size_t i = 0; i < row_count; i++) {
        fprintf(f, "            <tr>\n");
        fprintf(f, "                <td>%s</td>\n", rows[i].filename);
        fprintf(f, "                <td>%lld</td>\n", rows[i].bubble_ops);
        fprintf(f, "                <td>%lld</td>\n", rows[i].heap_ops);
        fprintf(f, "                <td>%lld</td>\n", rows[i].radix_ops);
        fprintf(f, "            </tr>\n");
    }
    fprintf(f, "        </table>\n");
    fprintf(f, "        <p>* Radixsort is a non-comparison-based sorting algorithm. The number of element-to-element comparisons is zero.</p>\n");
    fprintf(f, "        <table border=1>\n");
    fprintf(f, "            <tr>\n");
    fprintf(f, "                <th>File</th>\n");
    fprintf(f, "                <th>BubbleWrites</th>\n");
    fprintf(f, "                <th>HeapWrites</th>\n");
    fprintf(f, "                <th>RadixWrites</th>\n");
    fprintf(f, "            </tr>\n");
    for (size_t i = 0; i < row_count; i++) {
        fprintf(f, "            <tr>\n");
        fprintf(f, "                <td>%s</td>\n", rows[i].filename);
        fprintf(f, "                <td>%lld</td>\n", rows[i].bubble_writes);
        fprintf(f, "                <td>%lld</td>\n", rows[i].heap_writes);
        fprintf(f, "                <td>%lld</td>\n", rows[i].radix_writes);
        fprintf(f, "            </tr>\n");
    }
    fprintf(f, "        </table>\n");
    fprintf(f, "        <p>The *Writes* columns show the number of element assignments/writes performed by each algorithm.</p>\n");
    fprintf(f, "    </body>\n");
    fprintf(f, "</html>\n");

    fclose(f);
    return 1;
}

int main(int argc, char *argv[]) {
    const char *output_path = "results.html";
    int arg_start = 1;

    if (argc >= 4 && strcmp(argv[1], "--output") == 0) {
        output_path = argv[2];
        arg_start = 3;
    }

    if (argc <= arg_start) {
        print_usage(argv[0]);
        return 1;
    }

    size_t file_count = (size_t)(argc - arg_start);
    ResultRow *rows = calloc(file_count, sizeof(ResultRow));
    if (!rows) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    for (size_t i = 0; i < file_count; i++) {
        const char *input_path = argv[arg_start + (int)i];
        int *data = NULL;
        size_t size = 0;

        if (!load_file_data(input_path, &data, &size)) {
            for (size_t j = 0; j < i; j++) {
                free(rows[j].filename);
            }
            free(rows);
            return 1;
        }

        if (!measure_and_validate(bubble_sort_count_comparisons, data, size, &rows[i].bubble_ops, &rows[i].bubble_writes) ||
            !measure_and_validate(heap_sort_count_comparisons, data, size, &rows[i].heap_ops, &rows[i].heap_writes) ||
            !measure_and_validate(radix_sort_count_comparisons, data, size, &rows[i].radix_ops, &rows[i].radix_writes)) {
            fprintf(stderr, "Memory allocation or sorting validation failed for %s\n", input_path);
            free(data);
            for (size_t j = 0; j <= i; j++) {
                free(rows[j].filename);
            }
            free(rows);
            return 1;
        }
        rows[i].filename = duplicate_string(file_basename(input_path));

        if (!rows[i].filename) {
            fprintf(stderr, "Memory allocation failed while copying filename\n");
            free(data);
            for (size_t j = 0; j <= i; j++) {
                free(rows[j].filename);
            }
            free(rows);
            return 1;
        }

        printf("Processed %s (%zu values): bubble=%lld heap=%lld radix=%lld | writes: b=%lld h=%lld r=%lld\n",
               input_path, size, rows[i].bubble_ops, rows[i].heap_ops, rows[i].radix_ops,
               rows[i].bubble_writes, rows[i].heap_writes, rows[i].radix_writes);

        free(data);
    }

    if (!write_html_report(output_path, rows, file_count)) {
        for (size_t i = 0; i < file_count; i++) {
            free(rows[i].filename);
        }
        free(rows);
        return 1;
    }

    printf("HTML report generated: %s\n", output_path);

    for (size_t i = 0; i < file_count; i++) {
        free(rows[i].filename);
    }
    free(rows);

    return 0;
}
