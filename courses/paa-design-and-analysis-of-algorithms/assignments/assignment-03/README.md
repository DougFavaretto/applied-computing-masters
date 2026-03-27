# Assignment 03

## Problem

Implement and analyze the BubbleSort algorithm. The program must count and
output the number of operations performed to sort integer vectors.

### Input

- The program receives as a parameter the name of a text file
- The file contains integer values (one per line)
- The last line is blank, indicating the end of the list
- Test files may contain 100, 1,000, 10,000, or 100,000 values

### Output

**NOT** the sorted list. Instead, output a single numeric value representing
the total number of operations performed during the sorting process.

### Test Cases

Test your implementation with three types of vectors:
- Random values
- Sorted in ascending order
- Sorted in descending order

## Compilation and Execution

### Generating test data

Compile the data generator:
```bash
gcc -o generate-data generate-data.c
```

Generate the test files:
```bash
./generate-data --size 100000
```

This will create three files:
- `test_random_data.txt` - random data
- `test_sorted_asc_data.txt` - data sorted in ascending order
- `test_sorted_desc_data.txt` - data sorted in descending order

### Compiling BubbleSort

```bash
gcc -O2 -o bubble-sort bubble-sort.c
```

### Running the program

```bash
./bubble-sort --file ./test_random_data.txt
./bubble-sort --file ./test_sorted_asc_data.txt
./bubble-sort --file ./test_sorted_desc_data.txt
```

## Examples

After compiling the code, you can run the following command:

### Input

```bash
./generate-data --size 10000 && ./bubble-sort --file ./test_sorted_desc_data.txt
```

### Output

```text
Number of operations: 49995000
```