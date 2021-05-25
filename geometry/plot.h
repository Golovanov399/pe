#pragma once

#include "svg.h"

class Plot {
public:
	static constexpr int steps = 1000;

	Plot(ld _min_x, ld _max_x, ld _min_y, ld _max_y): min_x(_min_x), max_x(_max_x), min_y(_min_y), max_y(_max_y), width(min(max_x - min_x, max_y - min_y) / 100), svg() {
		ld w = max_x - min_x, h = max_y - min_y;
		svg.line(0, -h / 10, w, -h / 10, "black", width);
		svg.line(-w / 10, 0, -w / 10, h, "black", width);
		const ld fs = min(h / 10, w / 20);
		{
			string s = to_string(min_x);
			svg.text(s, 0, -2 * h / 10 - fs / 2, "black", fs);
		}
		{
			string s = to_string(max_x);
			svg.text(s, w, -2 * h / 10 - fs / 2, "black", fs);
		}
		{
			string s = to_string(min_y);
			svg.text(s, -2 * w / 10 - fs * s.size() / 2, 0, "black", fs);
		}
		{
			string s = to_string(max_y);
			svg.text(s, -2 * w / 10 - fs * s.size() / 2, h, "black", fs);
		}
	}

	template <typename Func>
	void plot(const Func& f, const string& color = "black") {
		Point<ld> last{min_x, f(min_x)};
		Segment<ld> low{Point<ld>{min_x, min_y}, Point<ld>{max_x, min_y}};
		Segment<ld> high{Point<ld>{min_x, max_y}, Point<ld>{max_x, max_y}};
		for (int i = 1; i < steps; ++i) {
			ld x = min_x + (max_x - min_x) / (steps - 1) * i;
			Point<ld> cur{x, f(x)};

			if (max(last.y, cur.y) >= min_y && min(last.y, cur.y) <= max_y) {
				Segment<ld> seg{last, cur};
				if (seg.p.y < min_y) {
					seg.p = *seg.intersection_point(low);
				}
				if (seg.p.y > max_y) {
					seg.p = *seg.intersection_point(high);
				}
				if (seg.q.y < min_y) {
					seg.q = *seg.intersection_point(low);
				}
				if (seg.q.y > max_y) {
					seg.q = *seg.intersection_point(high);
				}

				seg.p.x -= min_x;
				seg.p.y -= min_y;
				seg.q.x -= min_x;
				seg.q.y -= min_y;
				svg.draw(seg, color, width);
			}

			last = cur;
		}
	}

	void save(const string& filename) const {
		svg.save(filename);
	}

private:
	const ld min_x, max_x;
	const ld min_y, max_y;
	const ld width;

	SVG svg;
};
