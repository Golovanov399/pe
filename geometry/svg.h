#pragma once

#include "polygon.h"

class SVG {
public:
	static constexpr ld view_offset = .1;

	SVG() {
		ss << setprecision(6) << fixed;
		min_x = 1e18;
		max_x = -1e18;
		min_y = 1e18;
		max_y = -1e18;
	}

	void circle(ld x, ld y, ld r, string fill = "black", string outline = "", ld width = 1, ld alpha = 1.) {
		relax(x - r, y - r);
		relax(x + r, y + r);
		ss << "<circle cx=\"" << x << "\" cy=\"" << y << "\" r=\"" << r << "\"";
		finish_attributes(fill, outline, width, alpha);
	}

	void rect(ld x1, ld y1, ld x2, ld y2, string fill = "none", string outline = "black", ld width = 1, ld alpha = 1.) {
		relax(x1, y1);
		relax(x2, y2);
		if (x1 > x2) {
			swap(x1, x2);
		}
		if (y1 > y2) {
			swap(y1, y2);
		}
		ss << "<rect x=\"" << x1 << "\" y=\"" << y1 << "\" width=\"" << x2 - x1 << "\" height=\"" << y2 - y1 << "\"";
		finish_attributes(fill, outline, width, alpha);
	}

	void line(ld x1, ld y1, ld x2, ld y2, string outline = "black", ld width = 1, ld alpha = 1.) {
		relax(x1, y1);
		relax(x2, y2);
		ss << "<line x1=\"" << x1 << "\" y1=\"" << y1 << "\" x2=\"" << x2 << "\" y2=\"" << y2 << "\"";
		finish_attributes("", outline, width, alpha);
	}

	template <typename T>
	void polyline(const vector<Point<T>>& pts, string fill = "none", string outline = "black", ld width = 1, ld alpha = 1.) {
		for (const auto& p : pts) {
			relax(p);
		}
		ss << "<polyline points=\"";
		for (int i = 0; i < (int)pts.size(); ++i) {
			if (i) {
				ss << " ";
			}
			ss << pts[i].x << "," << pts[i].y;
		}
		ss << "\"";
		finish_attributes(fill, outline, width, alpha);
	}

	template <typename T>
	void draw(const Point<T>& p, ld r = 0.2, string fill = "black", string outline = "", ld width = 1, ld alpha = 1.) {
		circle(p.x, p.y, r, fill, outline, width, alpha);
	}

	template <typename T>
	void draw(const Segment<T>& s, string outline = "black", ld width = 1, ld alpha = 1.) {
		line(s.p.x, s.p.y, s.q.x, s.q.y, outline, width, alpha);
	}

	template <typename T>
	void draw(const Polygon<T>& poly, string fill = "none", string outline = "black", ld width = 1, ld alpha = 1.) {
		auto pts = poly.pts;
		pts.push_back(poly.pts[0]);
		polyline(pts, fill, outline, width, alpha);
	}

	void save(const string& filename) const {
		ofstream fstr(filename);
		const ld w = max_x - min_x, h = max_y - min_y;
		fstr << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>" << "\n";
		fstr << "<svg viewBox=\"" << min_x - w * view_offset << " " << min_y - h * view_offset << " " << max_x - min_x + 2 * w * view_offset << " " << max_y - min_y + 2 * h * view_offset << "\" xmlns=\"http://www.w3.org/2000/svg\">" << "\n";
		fstr << ss.str() << "\n";
		fstr << "</svg>\n";
	}

private:
	ld min_x, max_x, min_y, max_y;
	stringstream ss;

	void relax(ld x, ld y) {
		remin(min_x, x);
		remax(max_x, x);
		remin(min_y, y);
		remax(max_y, y);
	}

	template <typename T>
	void relax(const Point<T>& p) {
		relax(p.x, p.y);
	}

	void finish_attributes(string fill, string outline, ld width, ld alpha) {
		if (!fill.empty() && fill != "none") {
			ss << " fill=\"" << fill << "\"";
		} else {
			ss << " fill=\"none\" fill-opacity=\"0\"";
		}
		if (!outline.empty() && width > 0) {
			ss << " stroke-width=\"" << width << "\" stroke=\"" << outline << "\"";
		}
		if (alpha != 1.) {
			ss << " opacity=\"" << alpha << "\"";
		}
		ss << "/>\n";
	}
};