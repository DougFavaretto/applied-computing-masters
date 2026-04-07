# Assignment 04

## Questions

1. Write two versions of the Fibonacci algorithm: the presented "bad" version and a "good" version using arrays. Compare execution time for values between 25 and 60. What is the complexity of the "good" version?

2. Design a recursive algorithm to compute the square root of any positive real number. Write the recurrence relation and the corresponding worst-case complexity.

## How This Is Implemented

### Question 1 - Fibonacci

File: `fibonacci.c`

- `fibNaive`: bad version (recursive), exponential time.
- `fibArray`: good array-based version, linear time and linear space.
- `fibOptimal`: extra (bonus) version, linear time and constant space.

Complexity of the good version (`fibArray`):

- Time: `O(n)`
- Space: `O(n)`

Compile and run:

```bash
gcc -O2 -o fibonacci fibonacci.c
./fibonacci --fib 25
./fibonacci --fib 40
./fibonacci --fib 60
```

### Question 2 - Recursive Square Root

File: `square-root.c`

- Recursive binary search over a positive real number.
- Output with 3 decimal places.

Recurrence relation:

- `T(n) = T(n/2) + O(1)`

Complexity (worst case):

- Time: `O(log n)`

Compile and run:

```bash
gcc -O2 -o square-root square-root.c
./square-root --number 2.25
./square-root --number 50
```