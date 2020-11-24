#pragma once

#include "../base/base.h"

template <typename int_type>
struct Remainder {
	int_type mod;
	int_type rem;
};

template <typename int_type>
int_type inv(int_type a, int_type b) {
	return a == 1 ? 1 : b - inv(b % a, a) * b / a % b;
}

template <typename int_type>
pair<int_type, int_type> euc(int_type a, int_type b) {
	// returns such {x, y} that ax + by = gcd(a, b)
	auto g = gcd(a, b);
	auto x = inv(a / g, b / g);
	return {x, (g - a * x) / b};
}

template <typename int_type>
bool crt_once(Remainder<int_type>& r1, const Remainder<int_type>& r2) {
	auto g = gcd(r1.mod, r2.mod);
	if (r1.rem % g != r2.rem % g) {
		return false;
	}
	auto x = (r2.rem - r1.rem) % r2.mod;
	if (x < r2.mod) {
		x += r2.mod;
	}
	r1 = {lcm(r1.mod, r2.mod), r1.rem + x * inv(r1.mod / g, r2.mod / g) % r2.mod * (r1.mod / g)};
	return true;
}

template <typename int_type>
Remainder<int_type> crt(const vector<Remainder<int_type>>& remainders) {
	Remainder<int_type> ans{1, 0};
	for (const auto& r : remainders) {
		if (!crt_once(ans, r)) {
			return {-1, -1};
		}
	}
	return ans;
}