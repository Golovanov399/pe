#pragma once

#include "../../base/base.h"
#include "../../base/util.h"

template <typename T>
struct Point {
	T x, y;

	friend Point& operator +=(Point& p, const Point& q) {
		p.x += q.x;
		p.y += q.y;
		return p;
	}

	friend Point& operator -=(Point& p, const Point& q) {
		p.x -= q.x;
		p.y -= q.y;
		return p;
	}

	friend Point& operator *=(Point& p, T k) {
		p.x *= k;
		p.y *= k;
		return p;
	}

	friend Point& operator /=(Point& p, T k) {
		static_assert(!is_integral_v<T>);
		p.x /= k;
		p.y /= k;
		return p;
	}

	friend Point operator +(Point p, const Point& q) {
		p += q;
		return p;
	}

	friend Point operator -(Point p, const Point& q) {
		p -= q;
		return p;
	}

	friend Point operator *(Point p, T k) {
		p *= k;
		return p;
	}

	friend Point operator /(Point p, T k) {
		p /= k;
		return p;
	}

	T dot(const Point& q) const {
		return x * q.x + y * q.y;
	}

	T cross(const Point& q) const {
		return x * q.y - y * q.x;
	}

	T sprod(const Point& q) const {
		return dot(q);
	}

	T vprod(const Point& q) const {
		return cross(q);
	}

	friend T dot(const Point& p, const Point& q) {
		return p.dot(q);
	}

	friend T cross(const Point& p, const Point& q) {
		return p.cross(q);
	}

	friend T sprod(const Point& p, const Point& q) {
		return p.sprod(q);
	}

	friend T vprod(const Point& p, const Point& q) {
		return p.vprod(q);
	}

	T len() const {
		static_assert(!is_integral_v<T>);
		return hypot<T>(x, y);
	}

	T sqr() const {
		return dot(*this);
	}

	friend T len(const Point& p) {
		static_assert(!is_integral_v<T>);
		return p.len();
	}

	friend T dist(const Point& p, const Point& q) {
		static_assert(!is_integral_v<T>);
		return (p - q).len();
	}

	T dist(const Point& q) const {
		static_assert(!is_integral_v<T>);
		return (*this - q).len();
	}

	Point<T> rot90() const {
		return {-y, x};
	}

	Point<T> rotated(ld phi) const {
		static_assert(!is_integral_v<T>);
		ld cs = cos(phi), sn = sin(phi);
		return {x * cs - y * sn, x * sn + y * cs};
	}

	void rotate(ld phi) {
		*this = rotated(phi);
	}

	Point<T> rotated_via_trig(ld cs, ld sn) const {
		return {x * cs - y * sn, x * sn + y * cs};
	}

	void rotate_via_trig(ld cs, ld sn) {
		*this = rotated_via_trig(cs, sn);
	}

	template <typename U>
	Point<U> cast_to() const {
		return {static_cast<U>(x), static_cast<U>(y)};
	}

	bool operator ==(const Point& ot) const {
		if constexpr (is_integral_v<T>) {
			return tie(x, y) == tie(ot.x, ot.y);
		} else {
			return sign(x - ot.x) == 0 && sign(y - ot.y) == 0;
		}
	}

	bool operator !=(const Point& ot) const {
		return !(*this == ot);
	}

	bool operator <(const Point& ot) const {
		if constexpr (is_integral_v<T>) {
			return tie(x, y) < tie(ot.x, ot.y);
		} else {
			if (int sx = sign(x - ot.x); sx) {
				return sx < 0;
			} else {
				return sign(y - ot.y) < 0;
			}
		}
	}
};

template <typename T>
istream& operator >>(istream& istr, Point<T>& p) {
	return istr >> p.x >> p.y;
}

template <typename T>
ostream& operator <<(ostream& ostr, Point<T>& p) {
	if (&ostr == &cout) {
		return ostr << p.x << " " << p.y;
	} else {
		return ostr << "(" << p.x << ", " << p.y << ")";
	}
}
