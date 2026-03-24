# Assignment 02

## Problem

### A) Sorting algorithms

Implement the analyzed sorting algorithms and create a comparison table
of execution time for sorting:

- `n = 100,000` random numbers
- `n = 100,000` numbers already sorted in ascending order

### B) Additional algorithms

Develop the following algorithms, analyze their execution time for
different inputs, and determine their time complexity:

- Receive two integer values `a` and `b` and compute `a^b`
- Receive two square matrices of the same order (`n x n`) and perform
  their multiplication

## Reference Algorithms

### Bubble Sort

```c
void bubbleSort(int *v, int n) {
  int i, j, aux;
  for (i = n - 1; i > 0; i--) {
    for (j = 0; j < i; j++) {
      if (v[j] > v[j + 1]) {
      aux = v[j];
      v[j] = v[j + 1];
      v[j + 1] = aux;
      }
    }
  }
}
```

### Insertion Sort

```c
void insertionSort(int *v, int n) {
  int i, j, x;
  for (i = 1; i < n; i++) {
    x = v[i];
    j = i - 1;
    while (j >= 0 && v[j] > x) {
      v[j + 1] = v[j];
      j--;
    }
    v[j + 1] = x;
  }
}
```

### Selection Sort

```c
void selectionSort(int *v, int n) {
  int i, j, x, aux;
  for (i = 0; i < n; i++) {
    x = i;
    for (j = i + 1; j < n; j++) {
      if (v[j] < v[x])
      x = j;
    }
    aux = v[i];
    v[i] = v[x];
    v[x] = aux;
  }
}
```

## Run (Problem A)

### Compile

```bash
gcc -O2 -o problem_a problem_a.c -lm
```

### Execute

Use the compiled benchmark program:

```bash
./problem_a
```

By default, the program looks for input files in the current directory:

- `input_random_<size>.txt`
- `input_sorted_<size>.txt`

If these files exist, they are loaded. If they do not exist, they are
created automatically and reused in future runs.

Optional quick test (faster):

```bash
./problem_a --size 10000 --runs 2 --output-prefix results_a_quick
```

Generated files:

- `results_a.csv`
- `input_random_n.txt`
- `input_sorted_n.txt`

## Run (Problem B)

### Compile

```bash
gcc -O2 -o problem_b problem_b.c -lm
```

### Help

Display usage information:

```bash
./problem_b --help
```

### Execute - Power (a^b)

Benchmark exponentiation using recursive algorithm:

```bash
./problem_b power
```

With custom parameters:

```bash
./problem_b power --base 2 --exp 30 --runs 5 --output-prefix results_b_power_custom
```

Options:
- `--base A`: Base value (default: 2)
- `--exp B`: Exponent value (default: 30)
- `--runs R`: Number of runs (default: 3)
- `--output-prefix NAME`: Output CSV prefix (default: results_b_power)

### Execute - Matrix Multiplication

Benchmark n×n matrix multiplication:

```bash
./problem_b matrix
```

With custom parameters:

```bash
./problem_b matrix --size 500 --runs 3 --output-prefix results_b_matrix_large
```

Options:
- `--size N`: Matrix order n (default: 300)
- `--runs R`: Number of runs (default: 3)
- `--output-prefix NAME`: Output CSV prefix (default: results_b_matrix)

Default parameters:

- Power: base=2, exponent=30, runs=3
- Matrix: size=300×300, runs=3

Generated files:

- `results_b_power.csv` / `results_b_matrix.csv`
