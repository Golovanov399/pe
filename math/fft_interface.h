#pragma once

#include "../base/base.h"

template <typename outer_type, typename inner_type, int N>
class IFFT {
public:
	using Poly = vector<outer_type>;

	IFFT(): initialized_(false) {}

	~IFFT() {}

	virtual Poly multiply(Poly a, Poly b) {
		if ((int)a.size() > N / 2 || (int)b.size() > N / 2) {
			Poly result(a.size() + b.size() - 1);
			const int low_len = (max(a.size(), b.size()) + 1) / 2;
			Poly a_low(a.begin(), min(a.begin() + low_len, a.end()));
			Poly a_high(min(a.begin() + low_len, a.end()), a.end());
			Poly b_low(b.begin(), min(b.begin() + low_len, b.end()));
			Poly b_high(min(b.begin() + low_len, b.end()), b.end());

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
		Poly res((int)a.size() + (int)b.size() - 1);
		assert(res.size() <= ar.size());
		if constexpr (is_convertible_v<inner_type, outer_type>) {
			copy_n(ar.begin(), res.size(), res.begin());
		} else {
			throw runtime_error("please, implement your own child multiply function");
		}
		return res;
	}

	virtual Poly square(const Poly& a) {
		int n = 1;
		while (n < (int)a.size()) {
			n *= 2;
		}
		vector<inner_type> ar(n + n);
		if constexpr (is_convertible_v<outer_type, inner_type>) {
			copy(all(a), ar.begin());
		} else {
			throw runtime_error("please, implement your own child square function");
		}
		fft(ar);
		for (int i = 0; i < (int)ar.size(); ++i) {
			ar[i] *= ar[i];
		}
		ifft(ar);
		Poly res((int)a.size() + (int)a.size() - 1);
		assert(res.size() <= ar.size());
		if constexpr (is_convertible_v<inner_type, outer_type>) {
			copy_n(ar.begin(), res.size(), res.begin());
		} else {
			throw runtime_error("please, implement your own child square function");
		}
		return res;
	}

	Poly inverse(const Poly& a, int prec) {
		assert(!a.empty());
		assert(a[0] != 0);
		Poly b = {1 / a[0]};
		for (int len = 1; len < prec; len *= 2) {
			auto tmp = multiply(b, b);
			if ((int)tmp.size() > prec) {
				tmp.resize(prec);
			}
			tmp = multiply(tmp, Poly{a.begin(), a.begin() + min(2 * len, (int)a.size())});
			tmp.resize(2 * len);
			for (int i = 0; i < len; ++i) {
				tmp[i] = 2 * b[i] - tmp[i];
				tmp[len + i] = -tmp[len + i];
			}
			b.swap(tmp);
		}
		b.resize(prec);
		return b;
	}

	Poly derivative(Poly a) {
		if (a.empty()) {
			return a;
		}
		a.erase(a.begin());
		for (int i = 0; i < (int)a.size(); ++i) {
			a[i] *= i + 1;
		}
		return a;
	}

	Poly primitive(Poly a) {
		a.insert(a.begin(), 0);
		for (int i = 1; i < (int)a.size(); ++i) {
			a[i] /= i;
		}
		return a;
	}

	Poly log(const Poly& a, int prec) {
		assert(!a.empty());
		assert(a[0] == 1);
		auto res = primitive(multiply(derivative(a), inverse(a, prec)));
		res.resize(prec);
		return res;
	}

	Poly exp(const Poly& a, int prec) {
		assert(!a.empty());
		assert(a[0] == 0);
		Poly b = {1};
		for (int len = 1; len < prec; len *= 2) {
			auto tmp = Poly{a.begin(), a.begin() + min(2 * len, (int)a.size())};
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

	pair<Poly, Poly> divmod(Poly a, Poly b) {
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
		Poly r(b.size() - 1);
		auto bq = multiply(b, q);
		for (int i = 0; i < (int)r.size(); ++i) {
			r[i] = a[i] - bq[i];
		}
		return {q, r};
	}

	virtual Poly multipoint(Poly p, const vector<outer_type>& x) {
		vector<Poly> seg_prods;
		seg_prods.reserve(2 * (int)x.size() - 1);
		function<void(int, int)> push_prods = [&](int l, int r) {
			if (r == l + 1) {
				seg_prods.push_back({-x[l], 1});
				return;
			}
			int m = (l + r) / 2;
			push_prods(l, m);
			const auto& p = seg_prods.back();
			push_prods(m, r);
			const auto& q = seg_prods.back();	// no reallocations because of reserve()
			seg_prods.push_back(multiply(p, q));
		};
		push_prods(0, x.size());
		vector<outer_type> ans(x.size());
		auto fill_ans = [&](const auto& self, int l, int r, Poly p) {
			p = divmod(p, seg_prods.back()).second;
			seg_prods.pop_back();
			if (r <= l + 64) {
				for (int i = l; i < r; ++i) {
					outer_type& res = ans[i];
					for (int j = (int)p.size() - 1; j >= 0; --j) {
						res = res * x[i] + p[j];
					}
				}
				for (int i = l; i < r - 1; ++i) {
					seg_prods.pop_back();
					seg_prods.pop_back();
				}
				return;
			}
			int m = (l + r) / 2;
			self(self, m, r, p);
			self(self, l, m, p);
		};
		fill_ans(fill_ans, 0, x.size(), p);
		return ans;
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

	void butterfly(vector<inner_type>& a) {
		if (!initialized_) {
			initialize();
		}
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

	virtual void fft(vector<inner_type>& a) {
		if (!initialized_) {
			initialize();
		}

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

	void ifft(vector<inner_type>& a) {
		fft(a);
		for (auto& x : a) {
			x /= a.size();
		}
		reverse(1 + all(a));
	}
};
