#pragma once

#include "../base/traits.h"

struct Nimber {
	ull x;

	const static array<array<ull, 256>, 256> small;
	const static array<array<array<ull, 256>, 8>, 8> prec;

	Nimber(ull _x = 0): x(_x) {}

	static ull mult_pwrs(unsigned x, unsigned y) {
		if (!(x & y)) {
			return 1ull << (x ^ y);
		}
		auto z = x & y;
		ull ans = 1ull << (x ^ y);
		for (ull t = z; t; t ^= 1ull << __builtin_ctzll(t)) {
			unsigned b = __builtin_ctzll(t);
			b = 1u << b;
			ans = nim_mult_stupid(ans, (1ull << b) | (1ull << (b - 1)));
		}
		return ans;
	}

	static ull nim_mult_stupid(ull a, ull b) {
		ull ans = 0;
		for (ull aa = a; aa; aa ^= 1ull << __builtin_ctzll(aa)) {
			for (ull bb = b; bb; bb ^= 1ull << __builtin_ctzll(bb)) {
				ans ^= mult_pwrs(__builtin_ctzll(aa), __builtin_ctzll(bb));
			}
		}
		return ans;
	}

	Nimber operator +(const Nimber& ot) const {
		return x ^ ot.x;
	}

	Nimber operator *(const Nimber& ot) const {
		ull ans = 0;
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				ans ^= prec[i][j][small[(x >> (8 * i)) & 255][(ot.x >> (8 * j)) & 255]];
			}
		}
		return ans;
	}

	Nimber& operator +=(const Nimber& ot) {
		x ^= ot.x;
		return *this;
	}

	Nimber& operator *=(const Nimber& ot) {
		return *this = *this * ot;
	}
};

const array<array<ull, 256>, 256> Nimber::small = []() {
	array<array<ull, 256>, 256> res;
	for (int i = 0; i < 256; ++i) {
		for (int j = 0; j < 256; ++j) {
			res[i][j] = nim_mult_stupid(i, j);
		}
	}
	return res;
}();

const array<array<array<ull, 256>, 8>, 8> Nimber::prec = []() {
	array<array<array<ull, 256>, 8>, 8> res;
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			ull cur = mult_pwrs(8 * i, 8 * j);
			for (int k = 0; k < 256; ++k) {
				res[i][j][k] = nim_mult_stupid(cur, k);
			}
		}
	}
	return res;
}();

ull nim_multiply(ull a, ull b) {
	return (Nimber(a) * Nimber(b)).x;
}