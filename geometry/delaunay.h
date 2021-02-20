#pragma once

#include "primitives/all.h"

template <typename T>
class Delaunay {
public:
	struct Edge {
		Delaunay* par;
		int u, v;	// point ids
		int tid[2] = {-1, -1};	// triangle id
		int inner[2] = {-1, -1};	// position in triangle, (0, 1, 2)

		Edge(Delaunay* _par, int _u, int _v): par(_par), u(_u), v(_v) {}

		void change(int old_tid, int new_tid, int new_inner) {
			int idx = (old_tid == tid[1]);
			tid[idx] = new_tid;
			inner[idx] = new_inner;
		}

		bool contains(int i) const {
			const auto& pts = par->pts;
			if (u < 0 || v < 0) {
				return false;
			}
			if (u >= 0 && v >= 0) {
				return Segment<T>{pts[u], pts[v]}.contains(pts[i]);
			}
			assert(false);
		}

		int side(int i) const {
			const auto& pts = par->pts;
			// equivalent to the fact that (u, v, i) is a ccw triple
			if (u < 0 && v < 0) {
				return u == -2 ? 1 : -1;
			}
			if (u < 0 && i < 0) {
				return i == -2 ? 1 : -1;
			}
			if (v < 0 && i < 0) {
				return v == -2 ? 1 : -1;
			}
			if (i < 0) {
				return ((pts[u] < pts[v]) == (i == -1)) ? 1 : -1;
			}
			if (v < 0) {
				return ((pts[i] < pts[u]) == (v == -1)) ? 1 : -1;
			}
			if (u < 0) {
				return ((pts[v] < pts[i]) == (u == -1)) ? 1 : -1;
			}
			return sign(cross(pts[v] - pts[u], pts[i] - pts[u]));
		}

		int sum() const {
			return u + v;
		}
	};

	struct Triangle {
		Delaunay* par;
		int eid[3];
		vector<int> sons;

		Triangle(Delaunay* _par, int e1, int e2, int e3): par(_par) {
			eid[0] = e1;
			eid[1] = e2;
			eid[2] = e3;
		}

		array<int, 3> get_pids() const {
			const auto& edges = par->edges;
			int p1 = edges[eid[0]].u;
			int p2 = edges[eid[0]].v;
			if (edges[eid[1]].u == p1 || edges[eid[1]].v == p1) {
				swap(p1, p2);
			}
			int p0 = edges[eid[1]].sum() - p2;
			return {p0, p1, p2};
		}

		bool contains(int i) const {
			const auto& edges = par->edges;
			auto [p0, p1, p2] = get_pids();
			return edges[eid[0]].side(p0) * edges[eid[0]].side(i) >= 0 &&
				   edges[eid[1]].side(p1) * edges[eid[1]].side(i) >= 0 &&
				   edges[eid[2]].side(p2) * edges[eid[2]].side(i) >= 0;
		}

		int get_opposite(int eidx) const {
			const auto& edges = par->edges;
			return (edges[eid[(eidx + 1) % 3]].sum() + edges[eid[(eidx + 2) % 3]].sum() - edges[eid[eidx]].sum()) / 2;
		}

		int find(int u, int v) const {
			const auto& edges = par->edges;
			for (int i = 0; i < 3; ++i) {
				if (edges[eid[i]].sum() == u + v) {
					return i;
				}
			}
			assert(false);
		}
	};

	Delaunay(const vector<Point<T>>& _pts): pts(_pts) {
		int n = pts.size();
		vector<int> perm(n);
		iota(all(perm), 0);
		sort(all(perm), [&](int i, int j) {
			return pts[i] < pts[j];
		});
		random_shuffle(1 + all(perm));
		add_edge(-1, perm[0], 0, 0, -1, -1);
		add_edge(perm[0], -2, 0, 1, -1, -1);
		add_edge(-2, -1, 0, 2, -1, -1);
		add_triangle(0, 1, 2);
		perm.erase(perm.begin());
		for (int i : perm) {
			int tid = 0;
			while (true) {
				int nx = -1;
				for (int x : triangles[tid].sons) {
					if (triangles[x].contains(i)) {
						nx = x;
						break;
					}
				}
				if (nx == -1) {
					break;
				}
				tid = nx;
			}
			int on = -1;
			for (int j = 0; j < 3; ++j) {
				if (edges[triangles[tid].eid[j]].contains(i)) {
					on = triangles[tid].eid[j];
					break;
				}
			}

			vector<int> to_flip;

			if (on == -1) {
				auto [p0, p1, p2] = triangles[tid].get_pids();
				auto [e0, e1, e2] = triangles[tid].eid;
				int cur_t = triangles.size();
				int cur_e = edges.size();
				add_edge(i, p0, cur_t, 1, cur_t + 1, 2);
				add_edge(i, p1, cur_t + 1, 1, cur_t + 2, 2);
				add_edge(i, p2, cur_t + 2, 1, cur_t, 2);
				edges[e0].change(tid, cur_t + 2, 0);
				edges[e1].change(tid, cur_t, 0);
				edges[e2].change(tid, cur_t + 1, 0);
				add_triangle(e1, cur_e, cur_e + 2);
				add_triangle(e2, cur_e + 1, cur_e);
				add_triangle(e0, cur_e + 2, cur_e + 1);

				assert(triangles[tid].sons.empty());
				triangles[tid].sons = {cur_t, cur_t + 1, cur_t + 2};
				to_flip = {cur_t, cur_t + 1, cur_t + 2};
			} else {
				int u = edges[on].u, v = edges[on].v;
				int t0 = edges[on].tid[0];
				int t1 = edges[on].tid[1];
				int op0 = triangles[t0].get_opposite(edges[on].inner[0]);
				int op1 = triangles[t1].get_opposite(edges[on].inner[1]);

				int u0 = triangles[t0].eid[triangles[t0].find(u, op0)];
				int v0 = triangles[t0].eid[triangles[t0].find(v, op0)];
				int u1 = triangles[t1].eid[triangles[t1].find(u, op1)];
				int v1 = triangles[t1].eid[triangles[t1].find(v, op1)];

				int cur_t = triangles.size();
				int cur_e = edges.size();
				add_edge(i, u, cur_t, 2, cur_t + 2, 2);
				add_edge(i, v, cur_t + 1, 2, cur_t + 3, 2);
				add_edge(i, op0, cur_t, 1, cur_t + 1, 1);
				add_edge(i, op1, cur_t + 2, 1, cur_t + 3, 1);
				edges[u0].change(t0, cur_t, 0);
				edges[v0].change(t0, cur_t + 1, 0);
				edges[u1].change(t1, cur_t + 2, 0);
				edges[v1].change(t1, cur_t + 3, 0);
				add_triangle(u0, cur_e + 2, cur_e);
				add_triangle(v0, cur_e + 2, cur_e + 1);
				add_triangle(u1, cur_e + 3, cur_e);
				add_triangle(v1, cur_e + 3, cur_e + 1);

				assert(triangles[t0].sons.empty());
				assert(triangles[t1].sons.empty());
				triangles[t0].sons = {cur_t, cur_t + 1};
				triangles[t1].sons = {cur_t + 2, cur_t + 3};
				to_flip = {cur_t, cur_t + 1, cur_t + 2, cur_t + 3};
			}

			while (!to_flip.empty()) {
				int tid = to_flip.back();
				to_flip.pop_back();
				try_flip(to_flip, tid, i);
			}
		}
	}

	vector<pair<int, int>> get_edges() const {
		vector<char> used(edges.size());
		for (const auto& tr : triangles) {
			if (!tr.sons.empty()) {
				continue;
			}
			for (int i = 0; i < 3; ++i) {
				used[tr.eid[i]] = 1;
			}
		}
		vector<pair<int, int>> res;
		for (int i = 0; i < (int)edges.size(); ++i) {
			if (!used[i] || edges[i].u < 0 || edges[i].v < 0) {
				continue;
			}
			res.push_back({edges[i].u, edges[i].v});
		}
		return res;
	}

	vector<array<int, 3>> get_triangles() const {
		vector<array<int, 3>> res;
		for (const auto& tr : triangles) {
			if (!tr.sons.empty()) {
				continue;
			}
			if (auto pids = tr.get_pids(); *min_element(all(pids)) >= 0) {
				res.push_back(pids);
			}
		}
		return res;
	}

protected:
	void add_edge(int u, int v, int t1, int i1, int t2, int i2) {
		edges.push_back({this, u, v});
		if (t1 > -1) {
			edges.back().change(-1, t1, i1);
		}
		if (t2 > -1) {
			edges.back().change(-1, t2, i2);
		}
	}

	void add_triangle(int e1, int e2, int e3) {
		triangles.push_back({this, e1, e2, e3});
	}

	bool is_bad(int u, int v, int p0, int p1) {
		return (ld)abs(cross(pts[u] - pts[p0], pts[v] - pts[p0])) * dot(pts[u] - pts[p1], pts[v] - pts[p1]) +
			   (ld)abs(cross(pts[u] - pts[p1], pts[v] - pts[p1])) * dot(pts[u] - pts[p0], pts[v] - pts[p0]) < 0;
	}

	void try_flip(vector<int>& to_flip, int tid, int i) {
		int op = -1;
		for (int j = 0; j < 3; ++j) {
			if (i != edges[triangles[tid].eid[j]].u && i != edges[triangles[tid].eid[j]].v) {
				op = triangles[tid].eid[j];
				break;
			}
		}

		int t0 = edges[op].tid[0];
		int t1 = edges[op].tid[1];
		if (t0 == -1 || t1 == -1) {
			return;
		}
		int u = edges[op].u;
		int v = edges[op].v;
		int p0 = triangles[t0].get_opposite(triangles[t0].find(u, v));
		int p1 = triangles[t1].get_opposite(triangles[t1].find(u, v));

		if (p0 < 0 || p1 < 0) {
			return;
		}
		bool need_to_flip = false;
		if (u >= 0 && v >= 0) {
			need_to_flip = is_bad(u, v, p0, p1);
		} else if (u < 0) {
			need_to_flip = sign(cross(pts[v] - pts[p0], pts[p1] - pts[v])) * edges[op].side(p0) == -1;
		} else if (v < 0) {
			need_to_flip = sign(cross(pts[u] - pts[p0], pts[p1] - pts[u])) * edges[op].side(p0) == 1;
		} else {
			assert(false);
		}
		if (need_to_flip) {
			int u0 = triangles[t0].eid[triangles[t0].find(u, p0)];
			int v0 = triangles[t0].eid[triangles[t0].find(v, p0)];
			int u1 = triangles[t1].eid[triangles[t1].find(u, p1)];
			int v1 = triangles[t1].eid[triangles[t1].find(v, p1)];

			int cur_t = triangles.size();
			int cur_e = edges.size();

			add_edge(p0, p1, cur_t, 0, cur_t + 1, 0);
			edges[u0].change(t0, cur_t, 1);
			edges[v0].change(t0, cur_t + 1, 1);
			edges[u1].change(t1, cur_t, 2);
			edges[v1].change(t1, cur_t + 1, 2);
			add_triangle(cur_e, u0, u1);
			add_triangle(cur_e, v0, v1);

			assert(triangles[t0].sons.empty());
			assert(triangles[t1].sons.empty());
			triangles[t0].sons = {cur_t, cur_t + 1};
			triangles[t1].sons = {cur_t, cur_t + 1};

			to_flip.push_back(cur_t);
			to_flip.push_back(cur_t + 1);
		}
	}

protected:
	vector<Point<T>> pts;
	vector<Edge> edges;
	vector<Triangle> triangles;
};