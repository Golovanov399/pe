#pragma once

#include "traits.h"

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

li sqr(li x) {
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
