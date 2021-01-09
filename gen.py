#!/usr/bin/python3

n = 10**5
print(n)
a = [1]
while a[-1] * 2 < 10 ** 9 and len(a) < n:
	a.append(a[-1] * 2)
a.extend([a[-1]] * (n - len(a)))
print(*a)
