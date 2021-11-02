#pragma once

#include "../base/base.h"

struct WaveletLayer {
	vector<int> idx;
	vector<int> zeroes, ones;
	vector<int> lbs;
	vector<bool> highest;

	WaveletLayer() {}

	template <typename T>
	WaveletLayer(vector<T>& a, T bit) {
		vector<int> buf;
		highest.resize(a.size());
		idx.resize(a.size());
		lbs.resize((int)a.size() + 1);
		for (int i = 0, j = 0; i < (int)a.size(); ++i) {
			highest[i] = !!(a[i] & bit);
			idx[i] = (highest[i] ? ones : zeroes).size();
			(highest[i] ? ones : zeroes).push_back(i);
			if (highest[i]) {
				buf.push_back(a[i] ^ bit);
			} else {
				a[j++] = a[i];
			}
			lbs[i + 1] = lbs[i] + !highest[i];
		}
		copy(all(buf), a.begin() + zeroes.size());
	}

	int lower_bound(int z, int l) const {
		return z ? (int)zeroes.size() + l - lbs[l] : lbs[l];
	}

	bool get_kth_bit(int& l, int& r, int& k) const {
		auto lit = lower_bound(0, l);
		auto rit = lower_bound(0, r);
		if (k <= rit - lit) {
			l = lit;
			r = rit;
			return 0;
		} else {
			k -= rit - lit;
			l = lower_bound(1, l);
			r = lower_bound(1, r);
			return 1;
		}
	}

	template <typename T>
	int count_under(int& l, int& r, T& x, T bit) const {
		if (x < bit) {
			l = lower_bound(0, l);
			r = lower_bound(0, r);
			return 0;
		} else {
			int res = lower_bound(0, r) - lower_bound(0, l);
			l = lower_bound(1, l);
			r = lower_bound(1, r);
			x ^= bit;
			return res;
		}
	}
};

template <typename T = unsigned int, int N = sizeof(T) * 8>
struct WaveletMatrix {
	array<WaveletLayer, N> layers;
	vector<int> final_perm;

	template <typename S>
	WaveletMatrix(const vector<S>& a) {
		vector<T> ar(all(a));
		for (int i = 0; i < N; ++i) {
			layers[i] = WaveletLayer(ar, T(1) << (N - 1 - i));
		}
		vector<int> p(ar.size());
		iota(all(p), 0);
		for (int i = 0; i < N; ++i) {
			vector<int> q(p.size());
			for (int j = 0; j < (int)layers[i].zeroes.size(); ++j) {
				q[layers[i].zeroes[j]] = j;
			}
			for (int j = 0; j < (int)layers[i].ones.size(); ++j) {
				q[layers[i].ones[j]] = (int)layers[i].zeroes.size() + j;
			}
			for (int j = 0; j < (int)p.size(); ++j) {
				p[j] = q[p[j]];
			}
		}
		final_perm.resize(p.size());
		for (int i = 0; i < (int)p.size(); ++i) {
			final_perm[p[i]] = i;
		}
	}

	T get_kth(int l, int r, int k) const {
		assert(k <= r - l);
		T ans = 0;
		for (int i = 0; i < N; ++i) {
			ans |= T(layers[i].get_kth_bit(l, r, k)) << (N - 1 - i);
		}
		return ans;
	}

	int count_leq(int l, int r, T x) const {
		int ans = 0;
		for (int i = 0; i < N; ++i) {
			ans += layers[i].count_under(l, r, x, T(1) << (N - 1 - i));
		}
		return ans + (r - l);
	}

	int count_equal(int l, int r, T x) const {
		for (int i = 0; i < N; ++i) {
			layers[i].count_under(l, r, x, T(1) << (N - 1 - i));
		}
		return r - l;
	}

	int get_kth_x(int k, T x, int l, int r) const {
		for (int i = 0; i < N; ++i) {
			layers[i].count_under(l, r, x, T(1) << (N - 1 - i));
		}
		if (r - l < k) {
			return -1;
		}
		return final_perm[l + k - 1];
	}
};
