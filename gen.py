#!/usr/bin/python3

import random

t = 10000
print(t)
for i in range(t):
	q = random.choice("01")
	print(q)
	if q == '0':
		print(random.randint(1, 1000), random.randint(1, 1000))
	else:
		print(random.randint(1, 1000))
