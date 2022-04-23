#pragma once

#include <algorithm>
#include <ostream>
#include <tuple>
#include <type_traits>
#include <vector>
#include "../base/traits.h"
#include "prime.h"

using std::ostream, std::vector;

template <typename T = int>
struct Gaussian {
	static_assert(std::is_integral_v<T>);
	using S = next_size_t<T>;

	T x, y;

	Gaussian(T _x = 0, T _y = 0): x(_x), y(_y) {}

	Gaussian operator +(const Gaussian& ot) const {
		return {x + ot.x, y + ot.y};
	}

	Gaussian operator -(const Gaussian& ot) const {
		return {x - ot.x, y - ot.y};
	}

	Gaussian operator *(const Gaussian& ot) const {
		return {x * ot.x - y * ot.y, x * ot.y + y * ot.x};
	}

	Gaussian operator /(T k) const {
		assert(x % k == 0 && y % k == 0);
		return {x / k, y / k};
	}

	friend Gaussian operator /(const Gaussian& a, const Gaussian& b) {
		S n = b.norm();
		S r = (S)a.x * b.x + (S)a.y * b.y;
		S i = (S)a.y * b.x - (S)a.x * b.y;
		return {(T)(r / n), (T)(i / n)};
	}

	Gaussian& operator +=(const Gaussian& ot) {
		x += ot.x, y += ot.y;
		return *this;
	}

	Gaussian& operator -=(const Gaussian& ot) {
		x -= ot.x, y -= ot.y;
		return *this;
	}

	Gaussian& operator *=(const Gaussian& ot) {
		return *this = *this * ot;
	}

	Gaussian& operator /=(T k) {
		x /= k, y /= k;
		return *this;
	}

	friend Gaussian& operator /=(Gaussian& a, const Gaussian& b) {
		a = a / b;
		return a;
	}

	bool operator ==(const Gaussian<T>& ot) const {
		return tie(x, y) == tie(ot.x, ot.y);
	}

	bool operator !=(const Gaussian<T>& ot) const {
		return tie(x, y) != tie(ot.x, ot.y);
	}

	Gaussian conj() const {
		return {x, -y};
	}

	S norm() const {
		return (S)x * x + (S)y * y;
	}

	T real() const {
		return x;
	}

	T imag() const {
		return y;
	}

	bool is_divisible_by(const Gaussian& ot) const {
		S n = ot.norm();
		S r = (S)x * ot.x + (S)y * ot.y;
		S i = (S)y * ot.x - (S)x * ot.y;
		return r % n == 0 && i % n == 0;
	}

	bool divides(const Gaussian& ot) const {
		return ot.is_divisible_by(*this);
	}
};

template <typename T>
ostream& operator <<(ostream& ostr, const Gaussian<T>& z) {
	if (z.y == 0) {
		return ostr << "(" << z.x << ")";
	} else if (z.x == 0) {
		return ostr << "(" << z.y << "i)";
	} else {
		ostr << "(" << z.x;
		if (z.y < 0) {
			return ostr << z.y << "i)";
		} else {
			return ostr << "+" << z.y << "i)";
		}
	}
}

template <typename T>
Gaussian<T> find_gaussian_with_prime_norm(next_size_t<T> p) {
	if (p == 2) {
		return {1, 1};
	} else if (p % 4 == 3) {
		assert(false);
	}
	int g = 2;
	while (pw(g, (p - 1) / 2, p) != p - 1) {
		g += 1;
	}
	next_size_t<T> x = p, y = pw(g, (p - 1) / 4, p);
	while (p / x < x) {
		x %= y;
		swap(x, y);
	}
	return {(T)x, (T)y};
}

template <typename T>
vector<Gaussian<T>> factorize(Gaussian<T> z) {
	// returns a vector of gaussian primes
	// their product equals z up to invertibles
	vector<Gaussian<T>> res;
	auto n = z.norm();
	if (n <= 1) {
		return res;
	}
	auto ps = factorize(n);
	sort(ps.begin(), ps.end());
	long long last = -1;
	Gaussian<T> zl;
	for (auto p : ps) {
		if (p % 4 == 3) {
			res.push_back(p);
			continue;
		}
		if (p != last) {
			zl = find_gaussian_with_prime_norm<T>(p);
			last = p;
		}
		if (!z.is_divisible_by(zl)) {
			zl = zl.conj();
		}
		z /= zl;
		res.push_back(zl);
	}
	return res;
}