#pragma once

#include "../base/traits.h"
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

bool is_prime(long long n, int iters = 30) {
	// iters used to be 8, but it doesn't work here: https://dmoj.ca/problem/bf3hard
	if (n == 1) {
		return false;
	}
	if (n % 2 == 0) {
		return n == 2;
	}
	for (int i = 2; i < n && i < iters; ++i) {
		if (!miller_rabin(n, i)) {
			return false;
		}
	}
	return true;
}

// https://citeseerx.ist.psu.edu/viewdoc/download;jsessionid=0BBDB8D8B214E24EE2113ED076A8625F?doi=10.1.1.107.9984&rep=rep1&type=pdf
// https://codeforces.com/contest/1198/submission/58078132
ull squfof_iter(ull n, ull k, uint its) {
	if (gcd(k, n) != 1) {
		return gcd(k, n);
	}
	vector<uint> saved;
	ull nk = n * k;
	if (nk >> 62) {
		return 1;
	}
	uint sqrtn = isqrt(nk);
	uint cutoff = isqrt(2 * sqrtn);

	uint q0 = 1, p1 = sqrtn;
	uint q1 = nk - p1 * p1;
	if (q1 == 0) {
		ull d = gcd(n, p1);
		return d == n ? 1 : d;
	}

	uint mult = 2 * k;
	uint coarse_cutoff = mult * cutoff;

	uint i = 0, j = 0;
	uint p0 = 0;
	uint sqrtq = 0;

	for (i = 0; i < its; ++i) {
		uint q = 1, bits, tmp = sqrtn + p1 - q1;
		if (tmp >= q1) {
			q += tmp / q1;
		}
		p0 = q * q1 - p1;
		q0 += (p1 - p0) * q;

		if (q1 < coarse_cutoff) {
			tmp = q1 / gcd(q1, mult);
			if (tmp < cutoff) {
				saved.push_back(tmp);
			}
		}

		bits = __builtin_ctz(q0);
		tmp = q0 >> bits;
		if (!(bits & 1) && ((tmp & 7) == 1)) {
			sqrtq = isqrt(q0);
			if (sqr(sqrtq) == q0) {
				if (find(all(saved), sqrtq) == saved.end()) {
					break;
				}
			}
		}
		tmp = sqrtn + p0 - q0;
		q = 1;
		if (tmp >= q0) {
			q += tmp / q0;
		}
		p1 = q * q0 - p0;
		q1 += (p0 - p1) * q;

		if (q0 < coarse_cutoff) {
			tmp = q0 / gcd(q0, mult);
			if (tmp < cutoff) {
				saved.push_back(tmp);
			}
		}
	}

	if (sqrtq == 1) {
		return 1;
	}
	if (i == its) {
		return 1;
	}

	q0 = sqrtq;
	p1 = p0 + sqrtq * ((sqrtn - p0) / sqrtq);
	q1 = (nk - sqr<ull>(p1)) / q0;

	for (j = 0; j < its; ++j) {
		uint q = 1, tmp = sqrtn + p1 - q1;
		if (tmp >= q1) {
			q += tmp / q1;
		}
		p0 = q * q1 - p1;
		q0 += (p1 - p0) * q;

		if (p0 == p1) {
			q0 = q1;
			break;
		}

		tmp = sqrtn + p0 - q0;
		q = 1;
		if (tmp >= q0) {
			q += tmp / q0;
		}
		p1 = q * q0 - p0;
		q1 += (p0 - p1) * q;

		if (p0 == p1) {
			break;
		}
	}

	if (j == its) {
		return 1;
	}
	ull d = gcd(q0, n);
	return d == n ? 1 : d;
}

ull squfof(ull n) {
	if (ull x = icbrt(n); x * x * x == n) {
		return x;
	}
	for (int its = 1; its < 20000; its *= 4) {
		for (int p1 : {1, 3, 5, 3 * 5}) {
			for (int p2 : {1, 7, 11, 7 * 11}) {
				const int k = p1 * p2;
				if (ULLONG_MAX / k <= n) {
					continue;
				}
				ull d = squfof_iter(n, k, its * 300);
				if (d > 1 && d < n) {
					return d;
				}
			}
		}
	}
	assert(false);
}

vector<long long> factorize_rho(long long n) {
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
				auto tmp = factorize_rho(g);
				res.insert(res.end(), all(tmp));
				tmp = factorize_rho(n / g);
				res.insert(res.end(), all(tmp));
				return res;
			}
			x1 = (mult_big(x1, x1, n) + 1) % n;
			x2 = (mult_big(x2, x2, n) + 1) % n;
			x2 = (mult_big(x2, x2, n) + 1) % n;
		}
	}
}

vector<long long> factorize(long long n) {
	vector<long long> res;
	auto extract_all = [&](int p) {
		while (n % p == 0) {
			res.push_back(p);
			n /= p;
		}
	};
	for (int d : {2, 3, 5}) {
		extract_all(d);
	}
	for (int i = 6; i < 300; i += 6) {
		extract_all(i + 1);
		extract_all(i + 5);
	}
	if (n > 1) {
		vector<long long> st;
		st.push_back(n);
		while (!st.empty()) {
			n = st.back();
			st.pop_back();
			if (!is_prime(n)) {
				long long d = squfof(n);
				assert(d > 1 && d < n);
				st.push_back(d);
				st.push_back(n / d);
			} else {
				res.push_back(n);
			}
		}
	}
	return res;
}

vector<long long> get_divisors(long long n, bool rho = true) {
	auto fc = rho ? factorize_rho(n) : factorize(n);
	sort(all(fc));
	vector<long long> divs = {1ll};
	int sz = divs.size();
	long long last = 0;
	for (long long p : fc) {
		if (p != last) {
			sz = divs.size();
		}
		for (int i = 0; i < sz; ++i) {
			divs.push_back(divs[(int)divs.size() - sz] * p);
		}
		last = p;
	}
	return divs;
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
