#pragma once

#include <cmath>

#include "../base/traits.h"
#include "fft_interface.h"
#include "complex.h"

template <typename real_type, int N>
class FFTLL : public IFFT<real_type, complex<real_type>, N> {
	using base = complex<real_type>;
public:
	// Be careful, it's usually better to use 2 or 3 modulos NTT and then CRT (if vectors are ~5e5)
	vector<li> multiply(const vector<li>& a, const vector<li>& b) {
		static constexpr int block = 66666;
		if (!this->initialized_) {
			this->initialize();
		}
		const size_t size = max(1ul, a.size() + b.size()) - 1;
		int n = 1;
		while (n < (int)a.size() || n < (int)b.size()) {
			n *= 2;
		}
		vector<real_type> a_small(n + n), b_small(n + n);
		vector<real_type> a_large(n + n), b_large(n + n);
		vector<real_type> a_sum(n + n), b_sum(n + n);
		for (int i = 0; i < (int)a.size(); ++i) {
			a_large[i] = a[i] / block;
			a_small[i] = a[i] - round(a_large[i] * block);
			a_sum[i] = a_small[i] + a_large[i];
		}
		for (int i = 0; i < (int)b.size(); ++i) {
			b_large[i] = b[i] / block;
			b_small[i] = b[i] - round(b_large[i] * block);
			b_sum[i] = b_small[i] + b_large[i];
		}
		auto [ar_small, ar_large] = simultaneous_fft(a_small, a_large);
		auto [br_small, br_large] = simultaneous_fft(b_small, b_large);
		vector<base> ar_sum(n + n), br_sum(n + n);
		for (int i = 0; i < n + n; ++i) {
			ar_sum[i] = ar_small[i] + ar_large[i];
			br_sum[i] = br_small[i] + br_large[i];

			ar_small[i] *= br_small[i];
			ar_large[i] *= br_large[i];
			ar_sum[i] = ar_sum[i] * br_sum[i] - ar_small[i] - ar_large[i];

			ar_small[i] += ar_large[i] * base{0, 1};
		}
		this->ifft(ar_small);
		this->ifft(ar_sum);

		vector<li> res(size);
		for (int i = 0; i < (int)size; ++i) {
			li r1 = round(ar_small[i].real());
			li r2 = round(ar_small[i].imag());
			li r3 = round(ar_sum[i].real());
			res[i] = r1 + block * (r3 + block * r2);
		}
		return res;
	}

protected:
	void fill_angles() {
		const real_type pi = acosl(-1);
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
