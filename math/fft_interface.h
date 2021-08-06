#pragma once

#include "../base/base.h"

template <typename outer_type, typename inner_type, int N>
class IFFT {
public:
	IFFT(): initialized_(false) {}

	~IFFT() {}

	virtual vector<outer_type> multiply(vector<outer_type> a, vector<outer_type> b) {
		if ((int)a.size() > N / 2 || (int)b.size() > N / 2) {
			vector<outer_type> result(a.size() + b.size() - 1);
			const int low_len = (max(a.size(), b.size()) + 1) / 2;
			vector<outer_type> a_low(a.begin(), min(a.begin() + low_len, a.end()));
			vector<outer_type> a_high(min(a.begin() + low_len, a.end()), a.end());
			vector<outer_type> b_low(b.begin(), min(b.begin() + low_len, b.end()));
			vector<outer_type> b_high(min(b.begin() + low_len, b.end()), b.end());

			auto tmp = multiply(a_low, b_low);
			for (int i = 0; i < (int)tmp.size(); ++i) {
				result[i] += tmp[i];
				if (low_len + i < (int)result.size()) {
					result[low_len + i] -= tmp[i];
				}
			}
			tmp = multiply(a_high, b_high);
			for (int i = 0; i < (int)tmp.size(); ++i) {
				result[2 * low_len + i] += tmp[i];
				if (low_len + i < (int)result.size()) {
					result[low_len + i] -= tmp[i];
				}
			}
			for (int i = 0; i < (int)a_high.size(); ++i) {
				a_low[i] += a_high[i];
			}
			for (int i = 0; i < (int)b_high.size(); ++i) {
				b_low[i] += b_high[i];
			}
			tmp = multiply(a_low, b_low);
			for (int i = 0; i < (int)tmp.size(); ++i) {
				result[low_len + i] += tmp[i];
			}
			return result;
		}
		if (!initialized_) {
			initialize();
		}
		int n = 1;
		while (n < (int)a.size() || n < (int)b.size()) {
			n *= 2;
		}
		vector<inner_type> ar(n + n), br(n + n);
		if constexpr (is_convertible_v<outer_type, inner_type>) {
			copy(all(a), ar.begin());
			copy(all(b), br.begin());
		} else {
			throw runtime_error("please, implement your own child multiply function");
		}
		fft(ar);
		fft(br);
		for (int i = 0; i < (int)ar.size(); ++i) {
			ar[i] *= br[i];
		}
		ifft(ar);
		vector<outer_type> res((int)a.size() + (int)b.size() - 1);
		assert(res.size() <= ar.size());
		if constexpr (is_convertible_v<inner_type, outer_type>) {
			copy_n(ar.begin(), res.size(), res.begin());
		} else {
			throw runtime_error("please, implement your own child multiply function");
		}
		return res;
	}

	vector<outer_type> inverse(const vector<outer_type>& a, int prec) {
		assert(!a.empty());
		assert(a[0] != 0);
		vector<outer_type> b = {1 / a[0]};
		for (int len = 1; len < prec; len *= 2) {
			auto tmp = multiply(b, b);
			if ((int)tmp.size() > prec) {
				tmp.resize(prec);
			}
			tmp = multiply(tmp, vector<outer_type>{a.begin(), a.begin() + min(2 * len, (int)a.size())});
			tmp.resize(2 * len);
			cerr << a << " " << b << " " << tmp << "\n";
			for (int i = 0; i < len; ++i) {
				tmp[i] = 2 * b[i] - tmp[i];
				tmp[len + i] = -tmp[len + i];
			}
			b.swap(tmp);
		}
		b.resize(prec);
		return b;
	}

	vector<outer_type> derivative(vector<outer_type> a) {
		if (a.empty()) {
			return a;
		}
		a.erase(a.begin());
		for (int i = 0; i < (int)a.size(); ++i) {
			a[i] *= i + 1;
		}
		return a;
	}

	vector<outer_type> primitive(vector<outer_type> a) {
		a.insert(a.begin(), 0);
		for (int i = 1; i < (int)a.size(); ++i) {
			a[i] /= i;
		}
		return a;
	}

	vector<outer_type> log(const vector<outer_type>& a, int prec) {
		assert(!a.empty());
		assert(a[0] == 1);
		auto res = primitive(multiply(derivative(a), inverse(a, prec)));
		res.resize(prec);
		return res;
	}

	vector<outer_type> exp(const vector<outer_type>& a, int prec) {
		assert(!a.empty());
		assert(a[0] == 0);
		vector<outer_type> b = {1};
		for (int len = 1; len < prec; len *= 2) {
			auto tmp = vector<outer_type>{a.begin(), a.begin() + min(2 * len, (int)a.size())};
			tmp.resize(2 * len);
			tmp[0] += 1;
			auto l = log(b, 2 * len);
			for (int i = 0; i < 2 * len; ++i) {
				tmp[i] -= l[i];
			}
			b = multiply(tmp, b);
			b.resize(2 * len);
		}
		b.resize(prec);
		return b;
	}

	pair<vector<outer_type>, vector<outer_type>> divmod(vector<outer_type> a, vector<outer_type> b) {
		assert(!b.empty());
		assert(b.back() != 0);
		if (a.size() < b.size()) {
			return {{0}, a};
		}
		reverse(all(a));
		reverse(all(b));
		auto q = inverse(b, a.size() - b.size() + 1);
		q = multiply(a, q);
		q.resize(a.size() - b.size() + 1);
		reverse(all(q));
		reverse(all(a));
		reverse(all(b));
		vector<outer_type> r(b.size());
		auto bq = multiply(b, q);
		for (int i = 0; i < (int)r.size(); ++i) {
			r[i] = a[i] - bq[i];
		}
		return {q, r};
	}

protected:
	static constexpr int L = 31 - __builtin_clz(N);
	static_assert(!(N & (N - 1)));
	vector<inner_type> angles;
	vector<int> bitrev;
	bool initialized_;

	void initialize() {
		fill_angles();
		fill_bitrev();
		initialized_ = true;
	}

	virtual void fill_angles() = 0;

	void fill_bitrev() {
		bitrev.assign(N, 0);
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < L; ++j) {
				bitrev[i] = (bitrev[i] << 1) | !!(i & (1 << j));
			}
		}
	}

	void butterfly(vector<inner_type>& a) const {
		const int n = a.size();
		assert(!(n & (n - 1)));
		const int l = __builtin_ctz(n);

		for (int i = 0; i < n; ++i) {
			int j = revbit(i, l);
			if (j > i) {
				swap(a[i], a[j]);
			}
		}
	}

	int revbit(int num, int len) const {
		return bitrev[num] >> (L - len);
	}

	virtual void fft(vector<inner_type>& a) const {
		const int n = a.size();
		assert(!(n & (n - 1)));
		butterfly(a);

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

	void ifft(vector<inner_type>& a) const {
		fft(a);
		for (auto& x : a) {
			x /= a.size();
		}
		reverse(1 + all(a));
	}
};
