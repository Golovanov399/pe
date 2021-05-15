#pragma once

#include "../base/base.h"
#include "../base/util.h"

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
