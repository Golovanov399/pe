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
