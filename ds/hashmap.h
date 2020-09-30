#pragma once

#include "base.h"

template <typename K>
struct basic_hasher {
	unsigned int operator ()(const K& k, int n) const {
		static constexpr ull r = 11995408973635179863ull;
		return (k * r) >> (64 - n);
	}
};

template <typename K, typename V, typename H = basic_hasher<K>>
class HashMap {
private:
	int n;
	int sz;
	vector<ull> used;
	vector<pair<K, V>> vals;
	H hash;

	bool test(int i, const vector<ull>& u) const {
		return u[i >> 6] & (1ull << (i % 64));
	}

	void set(int i, vector<ull>& u) {
		u[i >> 6] |= 1ull << (i % 64);
	}

	int find_place(const K& k) const {
		int idx = hash(k, n);
		while (test(idx, used)) {
			if (vals[idx].first == k) {
				return idx;
			} else {
				idx = (idx + 1) & ((1 << n) - 1);
			}
		}
		return idx;
	}

	void reallocate() {
		n += 1;
		vector<ull> new_used(1 << (n - 6));
		vector<pair<K, V>> new_vals(1 << n);
		for (int i = 0; i < (int)vals.size(); ++i) {
			if (!test(i, used)) {
				continue;
			}
			int pos = hash(vals[i].first, n);
			while (test(pos, new_used)) {
				pos = (pos + 1) & ((1 << n) - 1);
			}
			set(pos, new_used);
			new_vals[pos] = vals[i];
		}
		new_vals.swap(vals);
		new_used.swap(used);
	}

public:
	HashMap(): n(10), sz(0), used(1 << (n - 6)), vals(1 << n), hash() {}

	V& operator [](const K& k) {
		int idx = find_place(k);
		if (test(idx, used)) {
			assert(vals[idx].first == k);
			return vals[idx].second;
		}
		++sz;
		if (sz * 3 / 2 >= (1 << n)) {
			reallocate();
			idx = find_place(k);
		}
		set(idx, used);
		vals[idx].first = k;
		return vals[idx].second;
	}

	const V& at(const K& k) const {
		int idx = find_place(k);
		assert(test(idx, used));
		return vals[idx].second;
	}

	const V& at(const K& k, const V& def) const {
		int idx = find_place(k);
		return test(idx, used) ? vals[idx].second : def;
	}

	size_t size() const {
		return sz;
	}
};
