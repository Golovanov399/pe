#!/usr/bin/python3

import re
import os.path

pattern = re.compile("^#include \".*\"$")

code = []
pasted = set()
once_lines = set()

def should_be_once(s):
	return s.strip() == "using namespace std;" or s.startswith("#include")

def handle(path):
	global pattern, code, pasted
	if path in pasted:
		return
	with open(path) as f:
		for line in f:
			if line.strip() == "#pragma once":
				pasted.add(path)
				continue
			if line in once_lines:
				continue
			elif should_be_once(line):
				once_lines.add(line)
			found = pattern.findall(line)
			if not found:
				code.append(line)
				continue
			assert len(found) == 1
			handle(os.path.normpath(os.path.join(os.path.dirname(path), found[0].strip()[10:-1])))

handle("qwe.cpp")
with open("out.cpp", "w") as f:
	print(*code, sep='', file=f)
