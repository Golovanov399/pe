#!/usr/bin/python3

import random

t = 100
print(t)
for i in range(t):
	n, m = random.randint(1, 10**9), random.randint(1, 10**9)
	a, b = random.randint(0, m - 1), random.randint(0, m - 1)
	print(n, m, a, b)
