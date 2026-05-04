# Assignment 5 – Sorting Algorithm Comparison

## Objective

This assignment asks students to implement sorting algorithms of different complexities covered in class and provide a numerical evaluation of their performance on several test cases.

---

## Assignment Description

Implement and compare the execution cost (measured as a number of operations) of the following sorting algorithms:
- BubbleSort
- Heapsort
- Radixsort

---

## Program Input

The program must sort vectors of integers provided via text files.

- Input is a list of `.txt` files passed as command-line arguments.
- Each file contains one integer per line; the last line is blank to indicate the end.
- The vector size is not provided in advance.

### Possible file sizes
- 100 values
- 1,000 values
- 10,000 values
- 100,000 values

### Vector types
- Random
- Sorted ascending
- Sorted descending

---

## Program Output

The program generates an HTML file containing a table that compares the measured costs (number of operations) for each algorithm.

---

## HTML Table Template

Example header used for the generated report:

```html
<html>
    <head>
        <title>PAA - Assignment 2</title>
    </head>
    <body>
        <table border=1>
            <tr>
                <th>File</th>
                <th>BubbleSort</th>
                <th>Heapsort</th>
                <th>Radixsort</th>
            </tr>
        </table>
    </body>
</html>
```

---

## Implementation

The program `sort_compare.c`:

- Accepts a list of `.txt` files as arguments.
- Runs BubbleSort, Heapsort and Radixsort on independent copies of the same input vector.
- Counts operations using the metric of element-to-element comparisons (for comparison-based algorithms).
- Produces an HTML report with one row per input file.

### Build

```bash
gcc -o sort-compare sort_compare.c
```

### Run

```bash
./sort-compare inputs/random_100.txt inputs/sorted_asc_100.txt
```

Optionally specify output filename:

```bash
./sort-compare --output results.html inputs/random_100.txt inputs/sorted_asc_100.txt
```

### Example with all input files

```bash
./sort-compare --output results.html inputs/*.txt
```

### Note about Radixsort

Radixsort is a non-comparison-based algorithm, therefore the element-to-element comparison counter for it is `0`.