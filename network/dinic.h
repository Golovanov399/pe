#pragma once

#include <vector>
#include <queue>
#include <limits>
#include <algorithm>

using std::vector, std::queue;

template <typename int_type = int>
class Dinic {
public:
	struct Edge {
		int from, to;
		int_type cap, flow;
	};

	int n;
	vector<Edge> edges;
	vector<vector<int>> eids;

	Dinic(int _n): n(_n), eids(_n) {}

	void add_edge(int u, int v, int_type c, bool bidirectional = false) {
		eids[u].push_back(edges.size());
		edges.push_back({u, v, c, 0});
		eids[v].push_back(edges.size());
		edges.push_back({v, u, bidirectional ? c : 0, 0});
	}

	int_type k_flow(int s, int t, int_type flow_limit) {
		int_type ans = 0;
		while (bfs(s, t)) {
			ptr.assign(n, 0);
			while (int_type new_flow = push_flow(s, t, flow_limit)) {
				ans += new_flow;
				flow_limit -= new_flow;
			}
			if (!flow_limit) {
				break;
			}
		}
		return ans;
	}

	int_type max_flow(int s, int t) {
		return k_flow(s, t, inf);
	}

private:
	vector<int_type> d;
	vector<int> ptr;
	static inline constexpr int_type inf = std::numeric_limits<int_type>::max();

	int_type push_flow(int s, int t, int_type flow_limit) {
		if (!flow_limit) {
			return 0;
		}
		if (s == t) {
			return flow_limit;
		}
		int_type res = 0;
		for (int& i = ptr[s]; i < (int)eids[s].size(); ++i) {
			int eid = eids[s][i];
			if (d[edges[eid].to] != d[s] + 1 || edges[eid].flow == edges[eid].cap) {
				continue;
			}
			int_type added_flow = push_flow(edges[eid].to, t, std::min(edges[eid].cap - edges[eid].flow, flow_limit));
			if (!added_flow) {
				continue;
			}
			edges[eid].flow += added_flow;
			edges[eid ^ 1].flow -= added_flow;
			flow_limit -= added_flow;
			res += added_flow;
			if (!flow_limit) {
				break;
			}
		}
		return res;
	}

	bool bfs(int s, int t) {
		d.assign(n, inf);
		d[s] = 0;
		queue<int> q;
		q.push(s);
		while (!q.empty()) {
			int v = q.front();
			q.pop();
			for (int eid : eids[v]) {
				if (edges[eid].flow == edges[eid].cap) {
					continue;
				}
				int to = edges[eid].to;
				if (d[to] > d[v] + 1) {
					d[to] = d[v] + 1;
					q.push(to);
				}
			}
		}
		return d[t] != inf;
	}
};