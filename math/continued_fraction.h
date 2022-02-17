#pragma once

#include <ostream>
#include <vector>

#include "../base/traits.h"
#include "crt.h"

using std::vector;

template <typename T>
struct ContinuedFraction {
	T x, y;

	ContinuedFraction operator +(const ContinuedFraction& ot) const {
		return {x + ot.x, y + ot.y};
	}

	ContinuedFraction operator *(T k) const {
		return {x * k, y * k};
	}
};

template <typename T>
std::ostream& operator <<(std::ostream& ostr, const ContinuedFraction<T>& f) {
	return ostr << f.x << "/" << f.y;
}

template <typename T, typename F>
vector<ContinuedFraction<T>> get_convergents(const F& cmp, T max_denom) {
	vector<ContinuedFraction<T>> res;
	ContinuedFraction<T> low = {1, 0}, high = {0, 1};
	res = {low, high};
	if (!cmp(low + high)) {
		swap(res[0], res[1]);
	}
	while (true) {
		bool need = cmp(low + high);
		auto p = low, q = high;
		if (!need) {
			swap(p, q);
		}
		T k = 1;
		while (cmp(p + q * k) == need) {
			k *= 2;
			if (p.x + q.x * k > max_denom) {
				break;
			}
		}
		while (k) {
			if (cmp(p + q * k) == need) {
				p = p + q * k;
			}
			k /= 2;
		}
		if (p.x > max_denom) {
			break;
		}
		res.push_back(p);
		(need ? low : high) = p;
	}
	return res;
}

template <typename T, typename F>
ContinuedFraction<T> get_best_approximation(const F& cmp, T max_denom) {
	auto vec = get_convergents(cmp, max_denom);
	if ((int)vec.size() == 1) {
		return vec.back();
	}
	return vec.back() + vec[(int)vec.size() - 2] * ((max_denom - vec.back().x) / vec[(int)vec.size() - 2].x);
}
/*
template <typename T>
ContinuedFraction<T> get_best_approximation(ld k, T max_denom) {
	assert(k > 0);
	return get_convergents([&](const ContinuedFraction<T>& cf) {
		return cf.y >= cf.x * k;
	}, max_denom).back();
}
*/
template <typename T, typename F>
T count_points_under(const F& cmp, T max_denom) {
	auto conv = get_convergents(cmp, max_denom);
	ContinuedFraction<T> cur{0, 0};
	T res = 1;
	auto add = [&](const ContinuedFraction<T>& r, T k) {
		res += cur.y * r.x * k + ((r.x * k + 1) * (r.y * k + 1) + k - 1) / 2;
		cur = cur + r * k;
	};
	while (!conv.empty()) {
		if (cur.x + conv.back().x > max_denom) {
			conv.pop_back();
			continue;
		}
		if (cmp(conv.back())) {
			add(conv.back(), (max_denom - cur.x) / conv.back().x);
		} else if ((int)conv.size() > 1 && conv.back().x) {
			add(conv[(int)conv.size() - 2], 1);
			add(conv.back(), (max_denom - cur.x) / conv.back().x);
		}
		conv.pop_back();
	}
	return res;
}