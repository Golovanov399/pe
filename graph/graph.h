#pragma once

#include "../base/base.h"
#include "../base/istream.h"
#include "../ds/skewheap.h"

enum GraphType {
	Undirected,
	Directed
};

enum EdgeType {
	Unit,
	Weighted
};

enum DistanceFindingAlgo {
	DijkstraPriorityQueue,
	DijkstraSet,
	DijkstraSkew,
	Dijkstra = DijkstraPriorityQueue,
	FordBellman,
	FordBellmanBfs
};

using EdgeInt = long long;

template <EdgeType etype>
struct Edge;

template <>
struct Edge<Unit> {
	int from, to;
};

template <>
struct Edge<Weighted> {
	int from, to;
	EdgeInt w;
};

template <EdgeType etype>
int other_end(const Edge<etype>& e, int v) {
	return e.from == v ? e.to : e.from;
}

template <GraphType gtype = Undirected, EdgeType etype = Unit>
class Graph {
public:
	Graph() {}
	explicit Graph(int _n): n(_n), a(_n) {}

	void read_edges(int edge_cnt, int start_index = 1) {
		for (int i = 0; i < edge_cnt; ++i) {
			int u = nxt() - start_index, v = nxt() - start_index;
			Edge<etype> e;
			e.from = u, e.to = v;
			if constexpr (etype == Weighted) {
				e.w = nxt<EdgeInt>();
			}
			a[e.from].push_back(edges.size());
			if constexpr (gtype == Undirected) {
				a[e.to].push_back(edges.size());
			}
			edges.push_back(e);
		}
	}

	void add_edge(int u, int v) {
		static_assert(etype == Unit);
		a[u].push_back(edges.size());
		if constexpr (gtype == Undirected) {
			a[v].push_back(edges.size());
		}
		edges.push_back({u, v});
	}

	void add_edge(int u, int v, EdgeInt w) {
		static_assert(etype == Weighted);
		a[u].push_back(edges.size());
		if constexpr (gtype == Undirected) {
			a[v].push_back(edges.size());
		}
		edges.push_back({u, v, w});
	}

	void set_n(int _n) {
		n = _n;
		a.resize(n);
	}

	int size() const {
		return n;
	}

	int edges_count() const {
		return edges.size();
	}

	vector<vector<int>> get_connected_components() {
		used.assign(n, false);
		vector<vector<int>> comps;
		auto add = [&](int v) {
			comps.back().push_back(v);
		};
		for (int i = 0; i < n; ++i) {
			if (!used[i]) {
				comps.push_back({});
				dfs(i, -1, {}, add, {});
			}
		}
		return comps;
	}

	vector<int> get_distances(int v) {
		static_assert(etype == Unit);
		vector<int> d, par;
		bfs(v, d, par);
		return d;
	}

	pair<vector<int>, vector<int>> get_distances_with_parents(int v) {
		static_assert(etype == Unit);
		vector<int> d, par;
		bfs(v, d, par);
		return {d, par};
	}

	vector<int> two_coloring() {
		static_assert(gtype == Undirected);
		used.assign(n, false);
		vector<int> color(n, 0);
		auto func = [&](int to, int from) {
			color[to] = color[from] ^ 1;
		};
		for (int i = 0; i < n; ++i) {
			if (!used[i]) {
				dfs(i, -1, func, {}, {});
			}
		}
		for (const auto& e : edges) {
			if (color[e.from] == color[e.to]) {
				return {};
			}
		}
		return color;
	}

	vector<EdgeInt> get_weighted_distances(int v, DistanceFindingAlgo algo = Dijkstra) {
		static_assert(etype == Weighted);
		vector<EdgeInt> d;
		vector<int> par;
		if (algo == DijkstraPriorityQueue) {
			dijkstra_pq(v, d, par);
		} else if (algo == DijkstraSet) {
			dijkstra_set(v, d, par);
		} else if (algo == DijkstraSkew) {
			dijkstra_skew(v, d, par);
		} else if (algo == FordBellman) {
			ford_bellman(v, d, par);
		} else if (algo == FordBellmanBfs) {
			ford_bellman_bfs(v, d, par);
		} else {
			assert(false);	// what the fuck
		}
		return d;
	}

	pair<vector<EdgeInt>, vector<int>> get_weighted_distances_with_parents(int v, DistanceFindingAlgo algo = Dijkstra) {
		static_assert(etype == Weighted);
		vector<EdgeInt> d;
		vector<int> par;
		if (algo == DijkstraPriorityQueue) {
			dijkstra_pq(v, d, par);
		} else if (algo == DijkstraSet) {
			dijkstra_set(v, d, par);
		} else if (algo == DijkstraSkew) {
			dijkstra_skew(v, d, par);
		} else if (algo == FordBellman) {
			ford_bellman(v, d, par);
		} else if (algo == FordBellmanBfs) {
			ford_bellman_bfs(v, d, par);
		} else {
			assert(false);	// what the fuck
		}
		return {d, par};
	}

	vector<vector<EdgeInt>> get_all_weighted_distances() {
		static_assert(etype == Weighted);
		constexpr EdgeInt inf = numeric_limits<EdgeInt>::max();
		vector<vector<EdgeInt>> d(n, vector<EdgeInt>(n, inf));
		for (int i = 0; i < n; ++i) {
			d[i][i] = 0;
			for (int eid : a[i]) {
				int j = other_end(edges[eid], i);
				d[i][j] = min(d[i][j], edges[eid].w);
			}
		}
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				for (int k = 0; k < n; ++k) {
					if (d[j][i] != inf && d[i][k] != inf) {
						d[j][k] = min(d[j][k], d[j][i] + d[i][k]);
					}
				}
			}
		}
		return d;
	}

private:
	int n;
	vector<vector<int>> a;
	vector<Edge<etype>> edges;

	vector<char> used;

	void dfs(int v, int pid, const optional<function<void(int, int)>>& pre, const optional<function<void(int)>>& in, const optional<function<void(int, int)>>& post) {
		used[v] = 1;
		if (in) {
			(*in)(v);
		}
		for (int eid : a[v]) {
			if (eid == pid) {
				continue;
			}
			int to = other_end(edges[eid], v);
			if (used[to]) {
				continue;
			}
			if (pre) {
				(*pre)(to, v);
			}
			dfs(to, eid, pre, in, post);
			if (post) {
				(*post)(to, v);
			}
		}
	}

	void bfs(int start, vector<int>& d, vector<int>& par) {
		d.assign(n, -1);
		par.assign(n, -1);
		d[start] = 0;
		queue<int> q;
		q.push(start);
		while (!q.empty()) {
			auto v = q.front();
			q.pop();
			for (int eid : a[v]) {
				int u = other_end(edges[eid], v);
				if (d[u] == -1) {
					d[u] = d[v] + 1;
					par[u] = v;
					q.push(u);
				}
			}
		}
	}

	void dijkstra_pq(int start, vector<EdgeInt>& d, vector<int>& par) {
		d.assign(n, numeric_limits<EdgeInt>::max());
		par.assign(n, -1);
		d[start] = 0;
		priority_queue<pair<EdgeInt, int>, vector<pair<EdgeInt, int>>, greater<pair<EdgeInt, int>>> pq;
		pq.push({0, start});
		while (!pq.empty()) {
			auto [val, v] = pq.top();
			pq.pop();
			if (d[v] != val) {
				continue;
			}
			for (int eid : a[v]) {
				assert(edges[eid].w >= 0);
				auto to = other_end(edges[eid], v);
				if (d[to] > val + edges[eid].w) {
					d[to] = val + edges[eid].w;
					par[to] = v;
					pq.push({d[to], to});
				}
			}
		}
	}

	void dijkstra_set(int start, vector<EdgeInt>& d, vector<int>& par) {
		d.assign(n, numeric_limits<EdgeInt>::max());
		par.assign(n, -1);
		d[start] = 0;
		set<pair<EdgeInt, int>> S;
		S.insert({0, start});
		while (!S.empty()) {
			auto [val, v] = *S.begin();
			S.erase(S.begin());
			for (int eid : a[v]) {
				assert(edges[eid].w >= 0);
				auto to = other_end(edges[eid], v);
				if (d[to] > val + edges[eid].w) {
					S.erase({d[to], to});
					d[to] = val + edges[eid].w;
					par[to] = v;
					S.insert({d[to], to});
				}
			}
		}
	}

	void dijkstra_skew(int start, vector<EdgeInt>& d, vector<int>& par) {
		d.assign(n, numeric_limits<EdgeInt>::max());
		par.assign(n, -1);
		d[start] = 0;
		SkewHeap<EdgeInt> heap(n);
		heap[start] = 0;
		heap.root = start;
		while (!heap.empty()) {
			auto val = heap.top();
			int v = heap.root;
			heap.pop();
			for (int eid : a[v]) {
				assert(edges[eid].w >= 0);
				auto to = other_end(edges[eid], v);
				if (par[to] == -1) {
					heap[to] = d[to] = val + edges[eid].w;
					par[to] = v;
					heap.root = heap.meld(heap.root, to);
				} else if (d[to] > val + edges[eid].w) {
					d[to] = val + edges[eid].w;
					par[to] = v;
					heap.update(to, d[to]);
				}
			}
		}
	}

	void ford_bellman(int start, vector<EdgeInt>& dist, vector<int>& par) {
		vector<int> order(n);
		iota(all(order), 0);
		{
			vector<int> d, p;
			bfs(start, d, p);
			sort(all(order), [&](int i, int j) {
				return d[i] < d[j];
			});
		}
		dist.assign(n, numeric_limits<EdgeInt>::max());
		par.assign(n, -1);
		dist[start] = 0;
		for (int iter = 0; iter < n; ++iter) {
			bool changed = false;
			for (int v : order) {
				for (int eid : a[v]) {
					int to = other_end(edges[eid], v);
					if (dist[to] > dist[v] + edges[eid].w) {
						changed = true;
						par[to] = v;
						dist[to] = dist[v] + edges[eid].w;
					}
				}
			}
			if (!changed) {
				return;
			}
		}
		throw logic_error("can't ford bellman when there is a negative cycle");
	}

	void ford_bellman_bfs(int start, vector<EdgeInt>& dist, vector<int>& par) {
		/* This doesn't detect negative cycles
		*/
		vector<char> in_queue(n);
		dist.assign(n, numeric_limits<EdgeInt>::max());
		par.assign(n, -1);
		dist[start] = 0;
		queue<int> q;
		q.push(start);
		in_queue[start] = true;
		while (!q.empty()) {
			int v = q.front();
			q.pop();
			in_queue[v] = false;
			for (int eid : a[v]) {
				int to = other_end(edges[eid], v);
				if (dist[to] > dist[v] + edges[eid].w) {
					dist[to] = dist[v] + edges[eid].w;
					par[to] = v;
					if (!in_queue[to]) {
						q.push(to);
						in_queue[to] = true;
					}
				}
			}
		}
	}
};


template <EdgeType etype = Unit>
using Ugraph = Graph<Undirected, etype>;

template <EdgeType etype = Unit>
using Digraph = Graph<Directed, etype>;
