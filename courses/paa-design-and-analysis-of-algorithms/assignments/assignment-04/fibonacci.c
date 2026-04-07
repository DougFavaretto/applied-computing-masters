#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

double get_time_seconds() {
    return (double)clock() / CLOCKS_PER_SEC;
}

void helper() {
    printf("This program calculates Fibonacci numbers using three approaches:\n");
    printf("  1. Naive recursive (exponential time)\n");
    printf("  2. Array-based (linear time)\n");
    printf("  3. Optimal (constant space)\n");
    printf("Usage: fibonacci [--help|-h] [--fib N]\n");
    printf("  --fib N   Calculate the n-th Fibonacci number (default: 25)\n");
}

/* Naive recursive implementation O(2^n) time, O(n) space (call stack) */
unsigned long long fibNaive(int n) {
    if (n == 0 || n == 1) return n;
    return fibNaive(n - 1) + fibNaive(n - 2);
}

/* Efficient array-based implementation O(n) time, O(n) space */
unsigned long long fibArray(int n) {
    if (n == 0 || n == 1) return n;
    unsigned long long *fib = (unsigned long long *)malloc((n + 1) * sizeof(unsigned long long));
    fib[0] = 0;
    fib[1] = 1;
    for (int i = 2; i <= n; i++) {
        fib[i] = fib[i - 1] + fib[i - 2];
    }
    unsigned long long result = fib[n];
    free(fib);
    return result;
}

/* Efficient implementation O(n) time, O(1) space */
unsigned long long fibOptimal( int n ){
    if ( n == 0 || n == 1 ) return n;
    unsigned long long a = 0ULL, b = 1ULL;
    for (int i = 2; i <= n; i++) {
        unsigned long long c = a + b;
        a = b;
        b = c;
    }
    return b;
}


int main(int argc, char *argv[]) {
    int n = 25; // default value
    char *end;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0){
            helper();
            return 0;
        }
        else if (strcmp(argv[i], "--fib") == 0 && i + 1 < argc) {
            n = strtol(argv[++i], &end, 10);
        }
        else {
            fprintf(stderr, "Unknown or incomplete argument: %s\n", argv[i]);
            helper();
            return 1;
        }
    }
    double start_time, end_time;

    start_time = get_time_seconds();
    printf("Array-based Fibonacci(%d) = %llu\n", n, fibArray(n));
    end_time = get_time_seconds();
    printf("Execution time (array): %f seconds\n\n", end_time - start_time);

    start_time = get_time_seconds();
    printf("Optimal Fibonacci(%d) = %llu\n", n, fibOptimal(n));
    end_time = get_time_seconds();
    printf("Execution time (optimal): %f seconds\n\n", end_time - start_time);
    
    start_time = get_time_seconds();
    printf("Naive Fibonacci(%d) = %llu\n", n, fibNaive(n));
    end_time = get_time_seconds();
    printf("Execution time (naive): %f seconds\n\n", end_time - start_time);

    return 0;
}