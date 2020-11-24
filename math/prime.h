#pragma once

#include "../base/base.h"
#include "modular.h"

bool is_prime(int n) {
	if (n == 1) {
		return false;
	}
	for (int i = 2; i * i <= n; ++i) {
		if (n % i == 0) {
			return false;
		}
	}
	return true;
}

bool is_square_residue(int a, int p) {
	assert(is_prime(p));
	if (!a) {
		return true;
	}
	if (p == 2) {
		return true;
	}
	if (integral_variable<int>::value != p) {
		set_prime(p);
	}
	return TypeModular<integral_variable<int>>(a).pow((p - 1) / 2) == 1;
}

int sqrt_mod(int a, int p) {
	if (!is_square_residue(a, p)) {
		return -1;
	}
	if (a == 0) {
		return 0;
	}
	if (p == 2) {
		return a;
	}
	using Mint = TypeModular<integral_variable<int>>;
	if (p % 4 == 3) {
		return (int)Mint(a).pow((p + 1) / 4);
	}

	// tonelli-shanks starts here
	Mint z = 1;
	while (z.pow((p - 1) / 2) == 1) {
		++z;
	}
	int q = p - 1;
	int s = __builtin_ctz(q);
	q >>= s;
	int m = s;
	Mint c = z.pow(q);
	Mint t = Mint(a).pow(q);
	Mint r = Mint(a).pow((q + 1) / 2);
	while (true) {
		if (t == 0) {
			return 0;
		}
		if (t == 1) {
			return (int)r;
		}
		auto tmp = t;
		int i = 0;
		while (tmp != 1) {
			i += 1;
			tmp *= tmp;
		}
		Mint b = c;
		for (int j = 0; j < m - i - 1; ++j) {
			b *= b;
		}
		r *= b;
		c = b * b;
		t *= c;
		m = i;
	}
}