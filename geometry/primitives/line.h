#pragma once

#include "point.h"

template <typename T>
struct Line {
	Point<T> p, v;	// line = {p + vt | t is real}

	bool is_parallel(const Line& ot) const {
		return sign(cross(v, ot.v)) == 0;
	}

	friend bool is_parallel(const Line& l1, const Line& l2) {
		return l1.is_parallel(l2);
	}

	friend bool are_parallel(const Line& l1, const Line& l2) {
		return l1.is_parallel(l2);
	}

	bool contains(const Point<T>& pt) const {
		return sign(cross(pt - p, v)) == 0;
	}

	Point<T> intersect(const Line& ot) const {
		static_assert(!is_integral_v<T>);
		assert(!is_parallel(ot));
		return p + v * (cross(ot.p - p, ot.v) / cross(v, ot.v));
	}

	friend Point<T> intersect(const Line& a, const Line& b) {
		return a.intersect(b);
	}

	template <typename U>
	Line<U> cast_to() const {
		return {p.template cast_to<U>(), v.template cast_to<U>()};
	}
};

template <typename T>
Line<T> line_through(const Point<T>& p, const Point<T>& q) {
	return {p, q - p};
}

template <typename T>
Line<T> segment_bisector(const Point<T>& p, const Point<T>& q) {
	static_assert(!is_integral_v<T>);
	return {(p + q) / 2, (q - p).rot90()};
}

template <typename T>
T dist(const Line<T>& l, const Point<T>& p) {
	static_assert(!is_integral_v<T>);
	return std::abs<T>((p - l.p).cross(l.v)) / l.v.len();
}

template <typename T>
inline T dist(const Point<T>& p, const Line<T>& l) {
	return dist(l, p);
}

template <typename T>
Line<T> angle_bisector(const Point<T>& a, const Point<T>& b, const Point<T>& c) {
	static_assert(!is_integral_v<T>);
	Point<T> u = a - b, v = c - b;
	u /= u.len();
	v /= v.len();
	v += u;
	auto l = v.len();
	if (!sign(l)) {
		return Line<T>{b, u.rot90()};
	} else {
		return Line<T>{b, v / l};
	}
}