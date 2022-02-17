#pragma once

#include <cassert>
#include <type_traits>
#include <ostream>
#include <utility>
#include <tuple>

using std::is_integral_v, std::is_floating_point_v;
using std::pair, std::ostream;

template <typename T>
bool is_zero(const T& x) {
	if (is_integral_v<T>) {
		return x == 0;
	} else if (is_floating_point_v<T>) {
		return abs(x) < 1e-8;
	} else {
		assert(false);
	}
}

template <typename real_type>
struct my_complex {
	real_type x, y;

	my_complex(real_type _x = 0, real_type _y = 0): x(_x), y(_y) {}

	my_complex operator +(const my_complex& ot) const {
		return {x + ot.x, y + ot.y};
	}

	my_complex operator -(const my_complex& ot) const {
		return {x - ot.x, y - ot.y};
	}

	my_complex operator *(const my_complex& ot) const {
		return {x * ot.x - y * ot.y, x * ot.y + y * ot.x};
	}

	my_complex operator /(real_type k) const {
		return {x / k, y / k};
	}

	friend my_complex operator /(const my_complex& a, const my_complex& b) {
		return a * b.conj() / b.norm();
	}

	my_complex& operator +=(const my_complex& ot) {
		x += ot.x, y += ot.y;
		return *this;
	}

	my_complex& operator -=(const my_complex& ot) {
		x -= ot.x, y -= ot.y;
		return *this;
	}

	my_complex& operator *=(const my_complex& ot) {
		std::tie(x, y) = pair{x * ot.x - y * ot.y, x * ot.y + y * ot.x};
		return *this;
	}

	my_complex& operator /=(real_type k) {
		x /= k, y /= k;
		return *this;
	}

	bool operator ==(const my_complex<real_type>& ot) const {
		return tie(x, y) == tie(ot.x, ot.y);
	}

	my_complex conj() const {
		return {x, -y};
	}

	real_type norm() const {
		return x * x + y * y;
	}

	real_type real() const {
		return x;
	}

	real_type imag() const {
		return y;
	}
};

template <typename real_type>
ostream& operator <<(ostream& ostr, const my_complex<real_type>& z) {
	if (is_zero(z.y)) {
		return ostr << "(" << z.x << ")";
	} else if (is_zero(z.x)) {
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

#define complex my_complex