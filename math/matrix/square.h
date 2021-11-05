#pragma once

#include "../../base/base.h"
#include "../../base/util.h"

template <typename T>
struct Matrix {
	int n;
	vector<vector<T>> a;

	explicit Matrix(int _n): n(_n), a(_n, vector<T>(_n)) {}

	static constexpr Matrix<T> id(int sz) {
		Matrix<T> res(sz);
		for (int i = 0; i < sz; ++i) {
			res[i][i] = 1;
		}
		return res;
	}

	struct GaussResult {
		bool success;
		vector<vector<T>> res;
		T det;
	};

	static constexpr GaussResult gauss(vector<vector<T>> a) {
		int n = a.size();
		vector<vector<T>> res(n, vector<T>(n));
		for (int i = 0; i < n; ++i) {
			res[i][i] = 1;
		}
		T det = 1;
		for (int i = 0; i < n; ++i) {
			if constexpr (is_floating_point_v<T>) {
				int mx = i;
				for (int j = i + 1; j < n; ++j) {
					if (abs(a[j][i]) > abs(a[mx][i])) {
						mx = j;
					}
				}
				if (sign(a[mx][i]) == 0) {
					return {false, res, 0};
				}
				if (mx > i) {
					a[mx].swap(a[i]);
					res[mx].swap(res[i]);
					det *= -1;
				}
			} else {
				if (a[i][i] == 0) {
					int j = i + 1;
					while (j < n && a[j][i] == 0) {
						++j;
					}
					if (j == n) {
						return {false, res, 0};
					}
					a[j].swap(a[i]);
					res[j].swap(res[i]);
					det *= -1;
				}
			}

			{
				T k = a[i][i];
				for (int j = i; j < n; ++j) {
					a[i][j] /= k;
				}
				for (int j = 0; j < n; ++j) {
					res[i][j] /= k;
				}
				det *= k;
			}
			for (int j = i + 1; j < n; ++j) {
				T k = a[j][i];
				if (k != 0) {
					for (int l = i; l < n; ++l) {
						a[j][l] -= a[i][l] * k;
					}
					for (int l = 0; l < n; ++l) {
						res[j][l] -= res[i][l] * k;
					}
				}
			}
		}

		for (int i = n - 1; i >= 0; --i) {
			for (int j = i - 1; j >= 0; --j) {
				T k = a[j][i];
				if (k != 0) {
					for (int l = 0; l < n; ++l) {
						res[j][l] -= res[i][l] * k;
					}
				}
			}
		}

		return {true, res, det};
	}

	const vector<T>& operator [](int idx) const {
		return a[idx];
	}

	vector<T>& operator [](int idx) {
		return a[idx];
	}

	void operator +=(const Matrix<T>& ot) {
		assert(n == ot.n);
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				a[i][j] += ot[i][j];
			}
		}
	}

	void operator -=(const Matrix<T>& ot) {
		assert(n == ot.n);
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				a[i][j] -= ot[i][j];
			}
		}
	}

	void operator *=(const T& k) {
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				a[i][j] *= k;
			}
		}
	}

	void operator /=(const T& k) {
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				a[i][j] /= k;
			}
		}
	}

	Matrix<T> operator +(const Matrix<T>& ot) const {
		auto res = *this;
		res += ot;
		return res;
	}

	Matrix<T> operator -(const Matrix<T>& ot) const {
		auto res = *this;
		res -= ot;
		return res;
	}

	Matrix<T> operator *(const T& k) const {
		auto res = *this;
		res *= k;
		return res;
	}

	Matrix<T> operator /(const T& k) const {
		auto res = *this;
		res /= k;
		return res;
	}

	Matrix<T> operator *(const Matrix<T>& ot) const {
		assert(n == ot.n);
		Matrix<T> res(n);
		for (int i = 0; i < n; ++i) {
			for (int k = 0; k < n; ++k) {
				for (int j = 0; j < n; ++j) {
					res[i][k] += a[i][j] * ot[j][k];
				}
			}
		}
		return res;
	}

	void operator *=(const Matrix<T>& ot) {
		*this = *this * ot;
	}

	T det() const {
		return gauss(a).det;
	}

	optional<Matrix<T>> inverse() const {
		if (auto res = gauss(a); res.success) {
			Matrix m(n);
			m.a = res.res;
			return {m};
		} else {
			return nullopt;
		}
	}

	static constexpr vector<T> characteristic(vector<vector<T>> a) {
		const int n = a.size();
		for (int i = 0; i < n - 1; ++i) {
			int j = i + 1;
			while (j < n && a[j][i] == 0) {
				++j;
			}
			if (j == n) {
				continue;
			}
			if (j > i + 1) {
				a[j].swap(a[i + 1]);
				for (int k = 0; k < n; ++k) {
					swap(a[k][j], a[k][i + 1]);
				}
			}
			T c = 1 / a[i + 1][i];
			for (int j = i + 2; j < n; ++j) {
				if (a[j][i] != 0) {
					T coef = c * a[j][i];
					for (int k = i; k < n; ++k) {
						a[j][k] -= a[i + 1][k] * coef;
					}
					for (int k = 0; k < n; ++k) {
						a[k][i + 1] += a[k][j] * coef;
					}
				}
			}
		}
		auto mult = [&](const vector<T>& a, const vector<T>& b) {
			vector<T> res(a.size() + b.size() - 1);
			for (int i = 0; i < (int)a.size(); ++i) {
				for (int j = 0; j < (int)b.size(); ++j) {
					res[i + j] += a[i] * b[j];
				}
			}
			return res;
		};
		vector<vector<T>> dp(n + 1);
		dp[0] = {1};
		for (int i = 0; i < n; ++i) {
			dp[i + 1] = mult(dp[i], {a[i][i], -1});
			T cur = 1;
			for (int j = i - 1; j >= 0; --j) {
				cur *= a[j + 1][j];
				int sign = (i % 2 == j % 2) ? 1 : -1;
				T coef = cur * a[j][i] * sign;
				for (int k = 0; k < (int)dp[j].size(); ++k) {
					dp[i + 1][k] += dp[j][k] * coef;
				}
			}
		}
		return dp[n];
	}

	vector<T> characteristic() const {
		return characteristic(a);
	}
};

template <typename T>
ostream& operator <<(ostream& ostr, const Matrix<T>& m) {
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

template <typename T>
istream& operator >>(istream& istr, Matrix<T>& m) {
	for (int i = 0; i < m.n; ++i) {
		for (int j = 0; j < m.n; ++j) {
			istr >> m[i][j];
		}
	}
	return istr;
}
