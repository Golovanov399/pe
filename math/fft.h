#pragma once

#include <cmath>

#include "fft_interface.h"
#include "complex.h"

template <typename real_type, int N>
class FFT : public IFFT<real_type, complex<real_type>, N> {
	using base = complex<real_type>;
public:
	FFT(): do_not_round(false) {}

	vector<real_type> multiply(vector<real_type> a, vector<real_type> b) {
		if (!this->initialized_) {
			this->initialize();
		}
		const size_t size = max(1ul, a.size() + b.size()) - 1;
		int n = 1;
		while (n < (int)a.size() || n < (int)b.size()) {
			n *= 2;
		}
		a.resize(n + n);
		b.resize(n + n);
		auto [ar, br] = simultaneous_fft(a, b);
		for (int i = 0; i < n + n; ++i) {
			ar[i] *= br[i];
		}
		this->ifft(ar);
		vector<real_type> res(size);
		for (size_t i = 0; i < size; ++i) {
			res[i] = do_not_round ? ar[i].real() : round(ar[i].real());
		}
		return res;
	}

	void set_rounding(bool val) {
		do_not_round = !val;
	}

	void cancel_rounding() {
		set_rounding(false);
	}

protected:
	bool do_not_round;

	void fill_angles() {
		const real_type pi = acos(-1);
		const base root{cos(2 * pi / N), sin(2 * pi / N)};
		this->angles.assign(N, 1);
		for (int i = 1; i < N; ++i) {
			this->angles[i] = this->angles[i - 1] * root;
			if (!(i & 8)) {
				this->angles[i] = {cos(2 * pi / N * i), sin(2 * pi / N * i)};
			}
		}
	}

	pair<vector<base>, vector<base>> simultaneous_fft(const vector<real_type>& a, const vector<real_type>& b) {
		assert(a.size() == b.size());
		const int n = a.size();
		assert(!(n & (n - 1)));
		vector<base> p(n);
		for (int i = 0; i < n; ++i) {
			p[i] = {a[i], b[i]};
		}
		this->fft(p);
		vector<base> ar(n), br(n);
		for (int i = 0; i < n; ++i) {
			auto x = p[i];
			auto y = i ? p[n - i] : p[0];
			y = y.conj();
			ar[i] = (x + y) / 2;
			br[i] = (x - y) / 2;
			swap(br[i].x, br[i].y);
			br[i].y *= -1;
		}
		return {ar, br};
	}
};
