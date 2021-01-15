#pragma once

#include "../base/functors.h"

template <typename T, class Op = Min<T>>
struct Sparse {
	Op op;
	vector<vector<T>> table;

	template <typename U = T>
	void build(const vector<U>& a) {
		table.assign(a.size(), {});
		for (int i = 0; i < (int)a.size(); ++i) {
			table[i] = {a[i]};
		}
		for (int l = 1, j = 0; 2 * l <= (int)a.size(); ++j, l <<= 1) {
			for (int i = 0; i + 2 * l <= (int)a.size(); ++i) {
				table[i].push_back(op(table[i][j], table[i + l][j]));
			}
		}
	}

	Sparse() {}

	template <typename U = T>
	explicit Sparse(const vector<U>& a) {
		build(a);
	}

	template <typename U = T>
	Sparse(const vector<U>& a, Op&& _op): op(_op) {
		build(a);
	}

	T get(int l, int r) const {
		assert(r > l);
		const int b = 31 - __builtin_clz(r - l);
		return op(table[l][b], table[r - (1 << b)][b]);
	}
};


template <typename T, class Op = Min<T>>
struct NonIdempotentSparse {
	Op op;
	vector<vector<T>> left, right;

	template <typename U = T>
	void build(const vector<U>& a) {
		left.assign(a.size(), {});
		right.assign(a.size(), {});
		for (int i = 0; i < (int)a.size(); ++i) {
			left[i] = {a[i]};
		}
		for (int i = 0; i < (int)a.size() - 1; ++i) {
			const int ctz = __builtin_ctz(i + 1);
			T cur = a[i];
			for (int j = i; j > 0 && __builtin_ctz(j) < ctz; --j) {
				cur = op(a[j - 1], cur);
				left[i].push_back(cur);
			}
		}
		for (int i = 1; i < (int)a.size(); ++i) {
			const int ctz = __builtin_ctz(i);
			T cur = a[i];
			right[i] = {cur};
			for (int j = i + 1; j < (int)a.size() && __builtin_ctz(j) < ctz; ++j) {
				cur = op(cur, a[j]);
				right[i].push_back(cur);
			}
		}
	}

	NonIdempotentSparse() {}

	template <typename U = T>
	explicit NonIdempotentSparse(const vector<U>& a) {
		build(a);
	}

	template <typename U = T>
	NonIdempotentSparse(const vector<U>& a, Op&& _op): op(_op) {
		build(a);
	}

	T get(int l, int r) const {
		assert(r > l);
		if (r == l + 1) {
			return left[l][0];
		}
		if (r == l + 2) {
			return op(left[l][0], right[r - 1][0]);
		}
		const int tmp = l ^ (r - 1);
		const int pos = (r - 1) & ~((1 << (31 - __builtin_clz(tmp))) - 1);
		return op(left[pos - 1][pos - l - 1], right[pos][r - pos - 1]);
	}
};
