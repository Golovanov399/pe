#pragma once

#include "traits.h"
#include "defines.h"
#include "sign.h"

#include <algorithm>
#include <cmath>
#include <vector>

using std::swap, std::vector;
using std::is_same_v, std::conditional_t;

#define _repeat_cat(a, b) a##b
#define _repeat_helper(ctr, n) for (int _repeat_cat(_mx_, ctr) = n, _repeat_cat(_i_, ctr) = 0; _repeat_cat(_i_, ctr) < _repeat_cat(_mx_, ctr); ++_repeat_cat(_i_, ctr))
#define repeat(n) _repeat_helper(__COUNTER__, n)

LI gcd(LI x, LI y) {
	while (y) {
		x %= y;
		swap(x, y);
	}
	return x;
}

LI lcm(LI x, LI y) {
	return x / gcd(x, y) * y;
}

LI abs(LI x) {
	return x > 0 ? x : -x;
}

template <typename T>
T pw(T a, li b) {
	T res = 1;
	while (b) {
		if (b & 1ll) {
			res *= a;
		}
		b >>= 1;
		a *= a;
	}
	return res;
}

li pw(li a, li b, int mod) {
	li res = 1;
	while (b) {
		if (b & 1ll) {
			res = res * a % mod;
		}
		b >>= 1;
		a = a * a % mod;
	}
	return res;
}

li mult_big(li x, li y, li mod) {
	li res = x * y - (li)((ld)x * (ld)y / (ld)mod) * mod;
	while (res < 0) {
		res += mod;
	}
	while (res >= mod) {
		res -= mod;
	}
	return res;
}

li pw_big(li a, li b, li mod) {
	li res = 1;
	while (b) {
		if (b & 1ll) {
			res = mult_big(res, a, mod);
		}
		b >>= 1;
		a = mult_big(a, a, mod);
	}
	return res;
}

template <typename T>
conditional_t<is_same_v<T, int>, long long, T> sqr(T x) {
	if constexpr (is_same_v<T, int>) {
		return 1ll * x * x;
	}
	return x * x;
}

template <typename T>
bool remin(T& x, T y) {
	if (x > y) {
		x = y;
		return true;
	} else {
		return false;
	}
}

template <typename T>
bool remax(T& x, T y) {
	if (x < y) {
		x = y;
		return true;
	} else {
		return false;
	}
}

int isqrt(long long n) {
	int x = sqrtl(n);
	while (sqr(x + 1) <= n) {
		++x;
	}
	return x;
}

int icbrt(long long n) {
	int x = pow(n, 1./3);
	while (sqr(x + 1) * (x + 1) <= n) {
		++x;
	}
	return x;
}

int mex(vector<int> a) {
	make_unique(a);
	int res = 0;
	while (res < (int)a.size() && a[res] == res) {
		++res;
	}
	return res;
}

template <typename T, typename Iter>
int find_position(Iter b, Iter e, const T& x) {
	auto it = lower_bound(b, e, x);
	if (it != e && *it == x) {
		return it - b;
	} else {
		return -1;
	}
}