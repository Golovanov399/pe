#pragma once

#include "../base/util.h"
#include "modular.h"

bool miller_rabin(long long n, long long a) {
	if (gcd(a, n) > 1) {
		return false;
	}
	long long d = n - 1;
	long long l = __builtin_ctzll(d);
	d >>= l;
	long long x = pw_big(a, d, n);
	if (x == 1) {
		return true;
	}
	while (l--) {
		if (x == n - 1) {
			return true;
		}
		x = mult_big(x, x, n);
	}
	return false;
}

bool is_prime(long long n) {
	if (n == 1) {
		return false;
	}
	if (n % 2 == 0) {
		return n == 2;
	}
	for (int i = 2; i < n && i < 8; ++i) {
		if (!miller_rabin(n, i)) {
			return false;
		}
	}
	return true;
}

vector<long long> factorize(long long n) {
	vector<long long> res;
	for (long long d : {2, 3, 5}) {
		while (n % d == 0) {
			n /= d;
			res.push_back(d);
		}
	}
	if (n == 1) {
		return res;
	}
	if (is_prime(n)) {
		res.push_back(n);
		return res;
	}
	while (true) {
		long long x1 = rand() % (n - 1) + 1;
		long long x2 = (mult_big(x1, x1, n) + 1) % n;
		while (true) {
			long long c = (x1 - x2 + n) % n;
			if (c == 0) {
				break;
			}
			long long g = gcd(c, n);
			if (g > 1) {
				auto tmp = factorize(g);
				res.insert(res.end(), all(tmp));
				tmp = factorize(n / g);
				res.insert(res.end(), all(tmp));
				return res;
			}
			x1 = (mult_big(x1, x1, n) + 1) % n;
			x2 = (mult_big(x2, x2, n) + 1) % n;
			x2 = (mult_big(x2, x2, n) + 1) % n;
		}
	}
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
	if (p % 4 == 3) {
		return pw(a, (p + 1) / 4, p);
	}

	// tonelli-shanks starts here
	int z = 1;
	while (pw(z, (p - 1) / 2, p) == 1) {
		++z;
	}
	int q = p - 1;
	int s = __builtin_ctz(q);
	q >>= s;
	int m = s;
	long long c = pw(z, q, p);
	long long t = pw(a, q, p);
	long long r = pw(a, (q + 1) / 2, p);
	while (true) {
		if (t == 0) {
			return 0;
		}
		if (t == 1) {
			return r;
		}
		auto tmp = t;
		int i = 0;
		while (tmp != 1) {
			i += 1;
			tmp = tmp * tmp % p;
		}
		long long b = c;
		for (int j = 0; j < m - i - 1; ++j) {
			b = b * b % p;
		}
		r = r * b % p;
		c = b * b % p;
		t = t * c % p;
		m = i;
	}
}