#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void helper() {
    printf("Usage: square-root [--help|-h] [--number N]\n");
    printf("  --number N   Calculate the square root of N\n");
}

double squareRootRecursive(double n, double low, double high) {
    if (high - low <= 0.0005) return (low + high) / 2.0;

    double mid = low + (high - low) / 2.0;
    double midSquared = mid * mid;
    if (midSquared < n) {
        return squareRootRecursive(n, mid, high);
    }
    return squareRootRecursive(n, low, mid);
}

int main(int argc, char *argv[]) {
    double n = 0;
    char *end;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0){
            helper();
            return 0;
        }
        else if (strcmp(argv[i], "--number") == 0 && i + 1 < argc) {
            n = strtod(argv[++i], &end);
            if (end == argv[i] || *end != '\0') {
                fprintf(stderr, "Invalid value for --number: %s\n", argv[i]);
                return 1;
            }
        }
        else {
            fprintf(stderr, "Unknown or incomplete argument: %s\n", argv[i]);
            helper();
            return 1;
        }
    }
    if (n == 0) {
        helper();
        return 0;
    } else if (n < 0) {
        fprintf(stderr, "Please provide a positive real number for N.\n");
        return 1;
    } else {
        double high = n >= 1.0 ? n : 1.0;
        double resultRecursive = squareRootRecursive(n, 0.0, high);
        printf("The square root of %.3f is approximately %.3f\n", n, resultRecursive);
        return 0;
    }
}