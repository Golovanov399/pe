#pragma once

#include <optional>

#include "../../base/util.h"
#include "line.h"

using std::optional, std::nullopt;
using std::is_integral_v;

template <typename T>
struct Segment {
	Point<T> p, q;

	bool is_parallel(const Segment& ot) const {
		return sign(cross(q - p, ot.q - ot.p)) == 0;
	}

	friend bool is_parallel(const Segment& l1, const Segment& l2) {
		return l1.is_parallel(l2);
	}

	friend bool are_parallel(const Segment& l1, const Segment& l2) {
		return l1.is_parallel(l2);
	}

	bool contains(const Point<T>& pt) const {
		return sign(cross(pt - p, pt - q)) == 0 && sign(dot(pt - p, pt - q)) <= 0;
	}

	optional<Point<T>> intersection_point(const Segment& ot) const {
		// only works if they are not on the same line!
		static_assert(!is_integral_v<T>);
		if (is_parallel(ot)) {
			return nullopt;
		}
		auto res = p + (q - p) * (cross(ot.p - p, ot.q - ot.p) / cross(q - p, ot.q - ot.p));
		return (contains(res) && ot.contains(res)) ? optional{res} : nullopt;
	}

	friend optional<Point<T>> intersection_point(const Segment& a, const Segment& b) {
		return a.intersection_point(b);
	}

	optional<Segment<T>> intersect(const Segment& ot) const {
		static_assert(!is_integral_v<T>);
		if (is_parallel(ot)) {
			bool p_lies = ot.contains(p);
			bool q_lies = ot.contains(q);
			bool ot_p_lies = contains(ot.p);
			bool ot_q_lies = contains(ot.q);
			if (p_lies && q_lies) {
				return *this;
			}
			if (ot_p_lies && ot_q_lies) {
				return ot;
			}
			if ((p_lies || q_lies) && (ot_p_lies || ot_q_lies)) {
				return Segment{p_lies ? p : q, ot_p_lies ? ot.p : ot.q};
			}
			return nullopt;
		}
		auto res = p + (q - p) * (cross(ot.p - p, ot.q - ot.p) / cross(q - p, ot.q - ot.p));
		return (contains(res) && ot.contains(res)) ? optional{Segment{res, res}} : nullopt;
	}

	friend Segment<T> intersect(const Segment& a, const Segment& b) {
		return a.intersect(b);
	}

	bool intersects(const Segment& ot) const {
		if (is_parallel(ot)) {
			bool p_lies = ot.contains(p);
			bool q_lies = ot.contains(q);
			bool ot_p_lies = contains(ot.p);
			bool ot_q_lies = contains(ot.q);
			return p_lies || q_lies || ot_p_lies || ot_q_lies;
		}
		return sign(cross(ot.p - p, q - p)) * sign(cross(ot.q - p, q - p)) <= 0 &&
			   sign(cross(p - ot.p, ot.q - ot.p)) * sign(cross(q - ot.p, ot.q - ot.p)) <= 0;
	}

	friend bool intersects(const Segment& a, const Segment& b) {
		return a.intersects(b);
	}

	template <typename U>
	Segment<U> cast_to() const {
		return {p.template cast_to<U>(), q.template cast_to<U>()};
	}
};
