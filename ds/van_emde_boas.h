#pragma once

#include <algorithm>
#include <array>
#include <type_traits>

using std::array, std::min, std::max;

template <int L, class D = void>
struct VanEmdeBoasNode {
	static constexpr int len = L / 2 * 6;
	static constexpr int inf = 1e9;
	using Smaller = VanEmdeBoasNode<(L >> 1)>;
	int mn, mx;
	array<Smaller, (1 << len)> sons;
	Smaller keys;

	constexpr VanEmdeBoasNode(): mn(inf), mx(-1) {}

	void insert(int key) {
		mn = ::min(mn, key);
		mx = ::max(mx, key);
		if (sons[high(key)].empty()) {
			keys.insert(high(key));
		}
		sons[high(key)].insert(low(key));
	}

	void erase(int key) {
		if (sons[high(key)].empty()) {
			return;
		}
		sons[high(key)].erase(low(key));
		if (sons[high(key)].empty()) {
			keys.erase(high(key));
		}
		if (mn == key) {
			if (mx == key) {
				mn = inf;
				mx = -1;
			} else {
				int x = keys.min();
				mn = (x << len) | sons[x].min();
			}
		} else if (mx == key) {
			int x = keys.max();
			mx = (x << len) | sons[x].max();
		}
	}

	inline bool empty() const {
		return mn == inf;
	}

	inline bool contains(int key) const {
		return sons[high(key)].contains(low(key));
	}

	inline int min() const {
		return mn == inf ? -1 : mn;
	}

	inline int max() const {
		return mx;
	}

	int find_next(int key) const {
		if (max() <= key) {
			return -1;
		}
		if (key < min()) {
			return min();
		}
		if (keys.contains(high(key)) && low(key) < sons[high(key)].max()) {
			return (high(key) << len) | sons[high(key)].find_next(low(key));
		} else {
			int x = keys.find_next(high(key));
			if (x == -1) {
				return -1;
			} else {
				return (x << len) | sons[x].min();
			}
		}
	}

	int find_prev(int key) const {
		if (min() >= key) {
			return -1;
		}
		if (key > max()) {
			return max();
		}
		if (keys.contains(high(key)) && low(key) > sons[high(key)].min()) {
			return (high(key) << len) | sons[high(key)].find_prev(low(key));
		} else {
			int x = keys.find_prev(high(key));
			if (x == -1) {
				return -1;
			} else {
				return (x << len) | sons[x].max();
			}
		}
	}

	int high(int key) const {
		return key >> len;
	}

	int low(int key) const {
		return key & ((1 << len) - 1);
	}
};


template <int L>
struct VanEmdeBoasNode<L, typename std::enable_if<L == 1>::type> {
	unsigned long long keys;

	VanEmdeBoasNode(): keys(0) {}

	void insert(int key) {
		keys |= 1ull << key;
	}

	void erase(int key) {
		keys &= ~(1ull << key);
	}

	inline bool empty() const {
		return !keys;
	}

	inline bool contains(int key) const {
		return !!(keys & (1ull << key));
	}

	inline int min() const {
		return keys ? __builtin_ctzll(keys) : -1;
	}

	inline int max() const {
		return keys ? 63 - __builtin_clzll(keys) : -1;
	}

	int find_next(int key) const {
		if (key == 63) {
			return -1;
		}
		auto tmp = keys & ~((1ull << (key + 1)) - 1);
		return tmp ? __builtin_ctzll(tmp) : -1;
	}

	int find_prev(int key) const {
		auto tmp = keys & ((1ull << key) - 1);
		return tmp ? 63 - __builtin_clzll(tmp) : -1;
	}
};

using VanEmdeBoas = VanEmdeBoasNode<4>;