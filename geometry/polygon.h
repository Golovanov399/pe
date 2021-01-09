#pragma once

#include "primitives/all.h"

template <typename T>
struct Polygon {
	vector<Point<T>> pts;
	vector<Segment<T>> sides;

	explicit Polygon(const vector<Point<T>>& _pts): pts(_pts) {
		sides.resize(pts.size());
		for (int i = 0; i < (int)pts.size(); ++i) {
			sides[i] = Segment<T>{pts[i], pts[nidx(i)]};
		}
	}

	int nidx(int i) const {
		return (i + 1) % (int)pts.size();
	}

	bool is_on_boundary(const Point<T>& p) const {
		for (const auto& side : sides) {
			if (side.contains(p)) {
				return true;
			}
		}
		return false;
	}

	bool is_inside(const Point<T>& p) const {
		if (is_on_boundary(p)) {
			return false;
		}
		T x_coord = p.x + 1;
		for (const auto& q : pts) {
			remax(x_coord, q.x + 1);
		}
		Point<T> q{x_coord, p.y};
		Segment<T> seg{p, q};
		int par = 0;
		for (const auto& side : sides) {
			if (seg.contains(side.p) && !seg.contains(side.q)) {
				par ^= 1;
			}
			if (seg.intersects(side)) {
				if (seg.contains(side.p) || seg.contains(side.q)) {
					par ^= sign(side.q.y - side.p.y) > 0;
				} else {
					par ^= 1;
				}
			}
		}
		return par;
	}

	T double_area() const {
		T res = 0;
		for (const auto& [p, q] : sides) {
			res += cross(p, q);
		}
		return abs(res);
	}

	size_t size() const {
		return pts.size();
	}

	Polygon convex_hull() const;
};

template <typename T>
Polygon<T> convex_hull(vector<Point<T>> pts) {
	make_unique(pts);
	if ((int)pts.size() <= 2) {
		return Polygon{pts};
	}

	{
		auto smallest = min_element(all(pts));
		swap(*smallest, pts[0]);
	}
	sort(1 + all(pts), [&](const Point<T>& p, const Point<T>& q) {
		if (auto cr = cross(p - pts[0], q - pts[0]); sign(cr)) {
			return cr > 0;
		} else {
			return (p - pts[0]).sqr() < (q - pts[0]).sqr();
		}
	});

	vector<Point<T>> npts = {pts[0], pts[1]};
	for (int i = 2; i < (int)pts.size(); ++i) {
		while ((int)npts.size() >= 2 && sign(cross(npts.back() - *next(npts.rbegin()), pts[i] - npts.back())) <= 0) {
			npts.pop_back();
		}
		npts.push_back(pts[i]);
	}

	return Polygon{npts};
}

template <typename T>
Polygon<T> Polygon<T>::convex_hull() const {
	return ::convex_hull(pts);
}