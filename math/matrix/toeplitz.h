#pragma once

#include <stdexcept>
#include "square.h"
#include "rect.h"

template <typename T>
struct Toeplitz {
	int n;
	vector<T> a;

	explicit Toeplitz(int _n): n(_n), a(2 * _n - 1) {}
	explicit Toeplitz(const vector<T>& vec): n(vec.size() / 2 + 1), a(vec) {
		assert(vec.size() % 2);
	}

	static constexpr Toeplitz<T> id(int sz) {
		Toeplitz<T> res(sz);
		res.a[sz - 1] = 1;
		return res;
	}

	struct row_view {
		int from;
		const Toeplitz* par;

		const T& operator [](int idx) {
			return par->a[from + par->n - 1 - idx];
		}
	};

	row_view operator [](int idx) const {
		return {idx, this};
	}

	void operator +=(const Toeplitz<T>& ot) {
		assert(n == ot.n);
		for (int i = 0; i < 2 * n - 1; ++i) {
			a[i] += ot.a[i];
		}
	}

	void operator -=(const Toeplitz<T>& ot) {
		assert(n == ot.n);
		for (int i = 0; i < 2 * n - 1; ++i) {
			a[i] -= ot.a[i];
		}
	}

	void operator *=(const T& k) {
		for (int i = 0; i < 2 * n - 1; ++i) {
			a[i] *= k;
		}
	}

	void operator /=(const T& k) {
		for (int i = 0; i < 2 * n - 1; ++i) {
			a[i] /= k;
		}
	}

	Toeplitz<T> operator +(const Toeplitz<T>& ot) const {
		auto res = *this;
		res += ot;
		return res;
	}

	Toeplitz<T> operator -(const Toeplitz<T>& ot) const {
		auto res = *this;
		res -= ot;
		return res;
	}

	Toeplitz<T> operator *(const T& k) const {
		auto res = *this;
		res *= k;
		return res;
	}

	Toeplitz<T> operator /(const T& k) const {
		auto res = *this;
		res /= k;
		return res;
	}

	Matrix<T> operator *(const Toeplitz<T>& ot) const {
		assert(n == ot.n);
		Matrix<T> res(n);
		// TODO vectorize somehow
		// TODO increase numerical stability
		for (int i = 0; i < n; ++i) {
			res[0][0] += a[n - 1 - i] * ot.a[n - 1 + i];
		}
		for (int i = 1; i < n; ++i) {
			for (int k = 0; k < n; ++k) {
				res[0][i] += a[n - 1 - k] * ot.a[n - 1 + k - i];
			}
			for (int k = 0; k < n; ++k) {
				res[i][0] += a[n - 1 - k + i] * ot.a[n - 1 + k];
			}
		}
		for (int i = 1; i < n; ++i) {
			for (int j = 1; j < n; ++j) {
				res[i][j] = res[i - 1][j - 1] + a[i + n - 1] * ot.a[n - 1 - j] - a[i - 1] * ot.a[2 * n - 1 - j];
			}
		}
		return res;
	}

	void transpose() {
		reverse(all(a));
	}

	vector<T> solve(const vector<T>& vec) const {
		if (a[n - 1] == 0) {
			throw std::logic_error("this Levinson-Durbin does not work with matrices with a nonsingular minor");
		}
		vector<T> f = {1 / a[n - 1]};
		vector<T> b = {1 / a[n - 1]};
		vector<T> nf, nb;
		vector<T> sol(n);
		sol[0] = f[0] * vec[0];
		for (int k = 2; k <= n; ++k) {
			T err_f = 0, err_b = 0;
			for (int i = 0; i < k - 1; ++i) {
				err_f += a[n - 1 + k - 1 - i] * f[i];
				err_b += a[n - 2 - i] * b[i];
			}
			if (err_f * err_b == 1) {
				throw std::logic_error("this Levinson-Durbin does not work with matrices with a nonsingular minor");
			}
			nf = f;
			nb = b;
			nf.push_back(0);
			nb.insert(nb.begin(), 0);
			for (int i = 0; i < k - 1; ++i) {
				nf[i + 1] -= b[i] * err_f;
				nb[i] -= f[i] * err_b;
			}
			T c = 1 - err_f * err_b;
			for (int i = 0; i < k; ++i) {
				nf[i] /= c;
				nb[i] /= c;
			}
			f.swap(nf);
			b.swap(nb);

			T need = vec[k - 1];
			for (int i = 0; i < k - 1; ++i) {
				need -= a[n - 1 + k - 1 - i] * sol[i];
			}
			for (int i = 0; i < k; ++i) {
				sol[i] += b[i] * need;
			}
		}
		for (int i = 0; i < n; ++i) {
			T res = vec[i];
			for (int j = 0; j < n; ++j) {
				res -= a[n - 1 + i - j] * sol[j];
			}
		}
		return sol;
	}

	Matrix<T> inverse() const {
		vector<T> f(n), e1(n);
		for (int i = 1; i < n; ++i) {
			f[n - i] = a[n - 1 - i] - a[2 * n - 1 - i];
		}
		e1[0] = 1;
		auto x = solve(f);
		auto y = solve(e1);
		Toeplitz<T> t1(n), u1(n), t2(n), u2(n);
		for (int i = 0; i < 2 * n - 1; ++i) {
			t1.a[i] = y[(i + 1) % n];
			t2.a[i] = x[(i + 1) % n];
			if (i == n - 1) {
				u1.a[i] = 1;
				u2.a[i] = 0;
			} else if (i < n - 1) {
				u1.a[i] = -x[i + 1];
				u2.a[i] = y[i + 1];
			}
		}
		return t1 * u1 + t2 * u2;
	}

	Matrix<T> as_matrix() const {
		Matrix<T> res(n);
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				res[i][j] = a[n - 1 + i - j];
			}
		}
		return res;
	}
};

template <typename T>
std::ostream& operator <<(std::ostream& ostr, const Toeplitz<T>& m) {
	for (int i = 0; i < m.n; ++i) {
		if (i) {
			ostr << "\n";
		}
		for (int j = 0; j < m.n; ++j) {
			if (j) {
				ostr << " ";
			}
			ostr << m[i][j];
		}
	}
	return ostr;
}
