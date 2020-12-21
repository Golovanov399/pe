#!/usr/bin/python3

n = int(input())
k = n//8*2
ed = []
for i in range(k):
	ed.append((i, i))
for i in range(k-1):
	ed.append((i+1, i))
for i in range(k//2):
	ed.append((i, k+2*i))
	ed.append((2*k+2*i, k+2*i))
	ed.append((2*k+2*i, 3*k+2*i))
	ed.append((k+2*i, i))
	ed.append((k+2*i, 2*k+2*i))
	ed.append((3*k+2*i, 2*k+2*i))
for j in range(k//2):
	i = k//2-1-j
	ed.append((k//2+j, k+2*i+1))
	ed.append((2*k+2*i+1, k+2*i+1))
	ed.append((2*k+2*i+1, 3*k+2*i+1))
	ed.append((k+2*i+1, k//2+j))
	ed.append((k+2*i+1, 2*k+2*i+1))
	ed.append((3*k+2*i+1, 2*k+2*i+1))

print(n, n, len(ed))
for u, v in ed:
	print(u, v)
