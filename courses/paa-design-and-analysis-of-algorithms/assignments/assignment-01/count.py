"""
This module contains a function to count the occurrences of an integer in a list,
as well as a main function to read input from stdin and print the result.
"""

import sys


def count_occurrences(seq: list[int], m: int) -> int:
    """Return the number of occurrences of m in seq."""
    return seq.count(m)


def _read_from_stdin() -> tuple[list[int], int]:

    tokens = sys.stdin.read().split()
    n = int(tokens[0])
    seq = list(map(int, tokens[1 : n + 1]))
    m = int(tokens[n + 1])
    return seq, m


def main() -> None:
    """Read input from stdin and print the count of occurrences."""
    seq, m = _read_from_stdin()
    print(count_occurrences(seq, m))


if __name__ == "__main__":
    main()
