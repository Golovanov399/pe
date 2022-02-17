#pragma once

#include <utility>
#include <numeric>

#include "../base/traits.h"
#include "../base/util.h"

using std::pair;
using std::gcd, std::lcm;

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
void extgcd(int_type& a, int_type& b, int_type x, int_type y) {
	if (y == 0) {
		a = 1;
		b = 0;
		return;
	}
	extgcd(b, a, y, x % y);
	b -= x / y * a;
}

template <typename int_type>
pair<int_type, int_type> extgcd(int_type x, int_type y) {
	int_type a, b;
	extgcd(a, b, x > 0 ? x : -x, y > 0 ? y : -y);
	if (x < 0) {
		a = -a;
	}
	if (y < 0) {
		b = -b;
	}
	return {a, b};
}

template <typename int_type>
pair<int_type, int_type> euc(int_type x, int_type y) {
	return extgcd(x, y);
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
	r1 = {lcm(r1.mod, r2.mod), r1.rem + (int_type)(x * (euc(r1.mod, r2.mod).first + r2.mod) % r2.mod) * (r1.mod / g)};
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