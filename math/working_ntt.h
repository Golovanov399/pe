#pragma once

#include "../base.h"
#include "modular.h"

template <int mod, int N = (1 << __builtin_ctz(mod - 1))>
struct NTT {
	static constexpr int L = 31 - __builtin_clz(N);
	using Mint = Modular<mod>;
	vector<Mint> angles;
	vector<int> bitrev;

	NTT() {
		static_assert(!(N & (N - 1)));
		vector<int> primes;
		for (int x = mod - 1, p = 2; x > 1; ++p) {
			if (p * p > x) {
				primes.push_back(x);
				break;
			}
			if (x % p == 0) {
				primes.push_back(p);
				while (x % p == 0) {
					x /= p;
				}
			}
		}
		auto isPrimitiveRoot = [&](Mint g) {
			for (int p : primes) {
				if (g.pow(mod / p) == 1) {
					return false;
				}
			}
			return true;
		};
		int g = 2;
		while (!isPrimitiveRoot(g)) {
			++g;
		}
		g = Mint(g).pow(mod / N).val;

		angles.assign(N, 1);
		for (int i = 1; i < N; ++i) {
			angles[i] = angles[i - 1] * g;
		}
		assert(angles.back() * g == 1);

		bitrev.assign(N, 0);
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < L; ++j) {
				bitrev[i] = (bitrev[i] << 1) | !!(i & (1 << j));
			}
		}
	}

	int revbit(int num, int len) const {
		return bitrev[num] >> (L - len);
	}

	void fft(vector<Mint>& a) const {
		const int n = a.size();
		assert(!(n & (n - 1)));
		const int l = __builtin_ctz(n);

		for (int i = 0; i < n; ++i) {
			int j = revbit(i, l);
			if (j > i) {
				swap(a[i], a[j]);
			}
		}

		for (int len = 1; len < n; len *= 2) {
			for (int start = 0; start < n; start += 2 * len) {
				for (int i = 0; i < len; ++i) {
					const auto w = angles[N / 2 / len * i];
					const auto x = a[start + i], y = a[start + len + i] * w;
					a[start + i] = x + y;
					a[start + len + i] = x - y;
				}
			}
		}
	}

	void ntt(vector<Mint>& a) const {
		fft(a);
	}

	vector<Mint> multiply(vector<Mint> a, vector<Mint> b, bool shorten = false) {
		const size_t size = max(1ul, a.size() + b.size()) - 1;
		int n = 1;
		while (n < (int)a.size() || n < (int)b.size()) {
			n *= 2;
		}
		a.resize(n + n);
		b.resize(n + n);
		fft(a);
		fft(b);
		for (int i = 0; i < n + n; ++i) {
			a[i] *= b[i];
		}
		fft(a);
		Mint inv = Mint(n + n).inv();
		for (auto& x : a) {
			x *= inv;
		}
		reverse(1 + all(a));
		a.resize(size);
		if (shorten) {
			while ((int)a.size() > 1 && !a.back()) {
				a.pop_back();
			}
		}
		return a;
	}
};
