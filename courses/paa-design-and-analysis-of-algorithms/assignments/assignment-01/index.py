"""
Assignment 01 — Design and Analysis of Algorithms (PAA)

Problem:
Given a sequence of n integers and a value m, count how many times m
appears in the sequence.

Constraints:
n < 1,000,000

Author: Douglas Favaretto
Program: Applied Computing Master's – UDESC
"""

import random
from count import count_occurrences

n = int(input("n (sequence length): "))
m = int(input("m (value to search): "))

input("Press Enter to generate the random sequence...")
seq = [random.randint(0, n) for _ in range(n)]

print("Generated sequence:", seq)
print(f"'{m}' appears {count_occurrences(seq, m)} time(s).")
