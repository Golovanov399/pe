#pragma once

#include "point.h"

template <typename T>
struct Circle {
	Point<T> p;
	T r;

	bool intersects(const Circle& ot) const {
		auto dst = (p - ot.p).sqr();
		return sign(dst - sqr(r - ot.r)) >= 0 && sign(dst - sqr(r + ot.r)) <= 0;
	}

	bool strictly_intersects(const Circle& ot) const {
		auto dst = (p - ot.p).sqr();
		if (sign(dst) == 0 && sign(r - ot.r) == 0) {
			return true;
		}
		return sign(dst - sqr(r - ot.r)) > 0 && sign(dst - sqr(r + ot.r)) < 0;
	}

	vector<Point<T>> intersect(const Circle& ot) const {
		static_assert(!is_integral_v<T>);
		if (!intersects(ot)) {
			return {};
		}
		if (p == ot.p) {
			// actually we may need to return an infinite number of pts, but we can't lol
			return {};
		}
		T d = dist(p, ot.p);
		T cos_phi = (sqr(d) + sqr(ot.r) - sqr(r)) / 2 / d / ot.r;
		if (sign(std::abs(cos_phi) - 1) == 0) {
			return {ot.p + (p - ot.p) * (ot.r / d) * cos_phi};
		}
		T sin_phi = sqrt(1 - sqr(cos_phi));
		auto vec = p - ot.p;
		vec *= (ot.r / d);
		return {ot.p + vec.rotated_via_trig(cos_phi, sin_phi),
				ot.p + vec.rotated_via_trig(cos_phi, -sin_phi)};
	}

	template <typename U>
	Circle<U> cast_to() const {
		return {p.template cast_to<U>(), static_cast<U>(r)};
	}
};