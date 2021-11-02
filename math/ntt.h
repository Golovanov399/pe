#pragma once

#include "../base/base.h"
#include "fft_interface.h"
// #include "modular.h"
#include "montgomery.h"

template <int mod, int N = (1 << __builtin_ctz(mod - 1))>
class NTT : public IFFT<Montgomery<mod>, Montgomery<mod>, N> {
	using Mint = Montgomery<mod>;
protected:
	void fill_angles() {
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
		g = Mint(g).pow(mod / N).get();

		this->angles.assign(N, 1);
		for (int i = 1; i < N; ++i) {
			this->angles[i] = this->angles[i - 1] * g;
		}
		assert(this->angles.back() * g == 1);
	}

	void ntt(vector<Mint>& a) {
		fft(a);
	}
};
