#pragma once

#include "../../base/base.h"
#include "../../base/util.h"

template <typename T>
struct RectMatrix {
	int n, m;
	vector<vector<T>> a;

	RectMatrix(int _n, int _m): n(_n), m(_m), a(_n, vector<T>(_m)) {}

	struct GaussResult {
		bool success;
		vector<vector<T>> lhs;
		vector<T> rhs;
		vector<int> main_coords;
	};

	static constexpr GaussResult gauss(vector<vector<T>> a, vector<T> b) {
		int n = a.size();
		int m = a[0].size();
		vector<int> main_coords;
		int i = 0;
		for (int col = 0; i < n && col < m; ++col) {
			if constexpr (is_floating_point_v<T>) {
				int mx = i;
				for (int j = i + 1; j < n; ++j) {
					if (abs(a[j][col]) > abs(a[mx][col])) {
						mx = j;
					}
				}
				if (sign(a[mx][col]) == 0) {
					continue;
				}
				if (mx > i) {
					a[mx].swap(a[i]);
					swap(b[mx], b[i]);
				}
			} else {
				if (a[i][col] == 0) {
					int j = i + 1;
					while (j < n && a[j][col] == 0) {
						++j;
					}
					if (j == n) {
						continue;
					}
					a[j].swap(a[i]);
					swap(b[j], b[i]);
				}
			}

			{
				T k = a[i][col];
				for (int j = col; j < m; ++j) {
					a[i][j] /= k;
				}
				b[i] /= k;
			}
			for (int j = i + 1; j < n; ++j) {
				T k = a[j][col];
				if (k != 0) {
					for (int l = col; l < m; ++l) {
						a[j][l] -= a[i][l] * k;
					}
					b[j] -= b[i] * k;
				}
			}
			main_coords.push_back(col);
			++i;
		}

		for (int row = i; row < n; ++row) {
			if constexpr (is_floating_point_v<T>) {
				if (sign(b[row])) {
					return {false, a, b, main_coords};
				}
			} else {
				if (b[row] != 0) {
					return {false, a, b, main_coords};
				}
			}
		}
		n = i;
		a.resize(n);
		b.resize(n);

		for (int i = n - 1; i >= 0; --i) {
			for (int j = i - 1; j >= 0; --j) {
				const int col = main_coords[i];
				T k = a[j][col];
				if (k != 0) {
					for (int l = 0; l < m; ++l) {
						a[j][l] -= a[i][l] * k;
					}
					b[j] -= b[i] * k;
				}
			}
		}

		return {true, a, b, main_coords};
	}

	const vector<T>& operator [](int idx) const {
		return a[idx];
	}

	vector<T>& operator [](int idx) {
		return a[idx];
	}

	void operator +=(const RectMatrix<T>& ot) {
		assert(n == ot.n);
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < m; ++j) {
				a[i][j] += ot[i][j];
			}
		}
	}

	void operator -=(const RectMatrix<T>& ot) {
		assert(n == ot.n);
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < m; ++j) {
				a[i][j] -= ot[i][j];
			}
		}
	}

	void operator *=(const T& k) {
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < m; ++j) {
				a[i][j] *= k;
			}
		}
	}

	void operator /=(const T& k) {
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < m; ++j) {
				a[i][j] /= k;
			}
		}
	}

	RectMatrix<T> operator +(const RectMatrix<T>& ot) const {
		auto res = *this;
		res += ot;
		return res;
	}

	RectMatrix<T> operator -(const RectMatrix<T>& ot) const {
		auto res = *this;
		res -= ot;
		return res;
	}

	RectMatrix<T> operator *(const T& k) const {
		auto res = *this;
		res *= k;
		return res;
	}

	RectMatrix<T> operator /(const T& k) const {
		auto res = *this;
		res /= k;
		return res;
	}

	RectMatrix<T> operator *(const RectMatrix<T>& ot) const {
		assert(m == ot.n);
		RectMatrix<T> res(n, ot.m);
		for (int i = 0; i < n; ++i) {
			for (int k = 0; k < ot.m; ++k) {
				for (int j = 0; j < m; ++j) {
					res[i][k] += a[i][j] * ot[j][k];
				}
			}
		}
		return res;
	}

	void operator *=(const RectMatrix<T>& ot) {
		*this = *this * ot;
	}

	optional<vector<vector<T>>> row_basis() const {
		if (auto res = gauss(a, vector<T>(n, 0)); res.success) {
			vector<vector<T>> ans;
			int i = 0;
			for (int c = 0; c < m; ++c) {
				if (i < (int)res.main_coords.size() && res.main_coords[i] == c) {
					++i;
					continue;
				} else {
					ans.emplace_back(m);
					ans.back()[c] = -1;
					for (int j = 0; j < (int)res.main_coords.size(); ++j) {
						ans.back()[res.main_coords[j]] = res.lhs[j][c];
					}
				}
			}
			return ans;
		} else {
			return {nullopt};
		}
	}

	optional<vector<T>> solve_linear_system(const vector<T>& rhs) const {
		assert((int)rhs.size() == n);
		if (auto res = gauss(a, rhs); res.success) {
			vector<T> ans(m);
			for (int i = 0; i < (int)res.main_coords.size(); ++i) {
				ans[res.main_coords[i]] = res.rhs[i];
			}
			return ans;
		} else {
			return nullopt;
		}
	}

	struct SolutionSpace {
		vector<T> solution;
		vector<vector<T>> basis;
	};

	optional<SolutionSpace> solve_linear_system_completely(const vector<T>& rhs) const {
		assert((int)rhs.size() == n);
		if (auto res = gauss(a, rhs); res.success) {
			vector<T> sol(m);
			for (int i = 0; i < (int)res.main_coords.size(); ++i) {
				sol[res.main_coords[i]] = res.rhs[i];
			}

			vector<vector<T>> basis;
			int i = 0;
			for (int c = 0; c < m; ++c) {
				if (i < (int)res.main_coords.size() && res.main_coords[i] == c) {
					++i;
					continue;
				} else {
					basis.emplace_back(m);
					basis.back()[c] = -1;
					for (int j = 0; j < (int)res.main_coords.size(); ++j) {
						basis.back()[res.main_coords[j]] = res.lhs[j][c];
					}
				}
			}

			return SolutionSpace{sol, basis};
		} else {
			return nullopt;
		}
	}
};

template <typename T>
ostream& operator <<(ostream& ostr, const RectMatrix<T>& m) {
	for (int i = 0; i < m.n; ++i) {
		if (i) {
			ostr << "\n";
		}
		for (int j = 0; j < m.m; ++j) {
			if (j) {
				ostr << " ";
			}
			ostr << m[i][j];
		}
	}
	return ostr;
}

template <typename T>
istream& operator >>(istream& istr, RectMatrix<T>& m) {
	for (int i = 0; i < m.n; ++i) {
		for (int j = 0; j < m.m; ++j) {
			istr >> m[i][j];
		}
	}
	return istr;
}
