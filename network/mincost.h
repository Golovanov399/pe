#pragma once

#include <vector>
#include <utility>
#include <limits>
#include <queue>

using namespace std;

template <typename flow_t, typename cost_t>
struct MinCostFlow {
	struct Edge {
		int from, to;
		flow_t cap, flow;
		cost_t cost;
	};

	static constexpr flow_t flow_inf = numeric_limits<flow_t>::max() / 2;
	static constexpr cost_t cost_inf = numeric_limits<cost_t>::max() / 2;

	int n;
	vector<Edge> edges;
	vector<vector<int>> a;

	explicit MinCostFlow(int _n): n(_n), a(_n) {}

	void add_edge(int u, int v, flow_t cap, cost_t cost) {
		a[u].push_back(edges.size());
		edges.push_back({u, v, cap, 0, cost});
		a[v].push_back(edges.size());
		edges.push_back({v, u, 0, 0, -cost});
	}

	vector<int> eid;
	vector<cost_t> dist;

	void ford_bellman(int s) {
		dist.assign(n, cost_inf);
		dist[s] = 0;
		while (true) {
			bool changed = false;
			for (int i = 0; i < (int)edges.size(); ++i) {
				const auto& e = edges[i];
				if (e.flow == e.cap) {
					continue;
				}
				if (dist[e.to] > dist[e.from] + e.cost) {
					dist[e.to] = dist[e.from] + e.cost;
					changed = true;
				}
			}
			if (!changed) {
				break;
			}
		}
	}

	bool build_path(int s, int t) {
		eid.assign(n, -1);
		vector<cost_t> d(n, cost_inf);
		d[s] = 0;
		priority_queue<pair<cost_t, int>, vector<pair<cost_t, int>>, greater<>> pq;
		pq.push({0, s});
		while (!pq.empty()) {
			auto [dst, v] = pq.top();
			pq.pop();
			if (d[v] != dst) {
				continue;
			}
			for (int i : a[v]) {
				const auto& e = edges[i];
				if (e.cap == e.flow) {
					continue;
				}
				if (d[e.to] - dist[e.to] > dst - dist[v] + e.cost) {
					d[e.to] = dst - dist[v] + e.cost + dist[e.to];
					eid[e.to] = i;
					pq.push({d[e.to], e.to});
				}
			}
		}
		for (int i = 0; i < n; ++i) {
			dist[i] -= d[i];
		}
		return eid[t] > -1;
	}

	cost_t min_cost_k_flow(int s, int t, flow_t flow) {
		ford_bellman(s);
		cost_t ans = 0;
		while (flow > 0 && build_path(s, t)) {
			cost_t cost = 0;
			flow_t cur_flow = flow;
			for (int x = t; x != s; x = edges[eid[x]].from) {
				const int id = eid[x];
				cost += edges[id].cost;
				cur_flow = min(cur_flow, edges[id].cap - edges[id].flow);
			}
			flow -= cur_flow;
			ans += cur_flow * cost;
			for (int x = t; x != s; x = edges[eid[x]].from) {
				const int id = eid[x];
				edges[id].flow += cur_flow;
				edges[id ^ 1].flow -= cur_flow;
			}
		}
		if (flow > 0) {
			return cost_inf;
		}
		return ans;
	}

	pair<flow_t, cost_t> min_cost_max_flow(int s, int t) {
		ford_bellman(s);
		flow_t total_flow = 0;
		cost_t total_cost = 0;
		while (build_path(s, t)) {
			cost_t cost = 0;
			flow_t cur_flow = flow_inf;
			for (int x = t; x != s; x = edges[eid[x]].from) {
				const int id = eid[x];
				cost += edges[id].cost;
				cur_flow = min(cur_flow, edges[id].cap - edges[id].flow);
			}
			if (cur_flow == 0) {
				break;
			}
			total_flow += cur_flow;
			total_cost += cur_flow * cost;
			for (int x = t; x != s; x = edges[eid[x]].from) {
				const int id = eid[x];
				edges[id].flow += cur_flow;
				edges[id ^ 1].flow -= cur_flow;
			}
		}
		return {total_flow, total_cost};
	}

	pair<flow_t, cost_t> min_cost_any_flow(int s, int t) {
		ford_bellman(s);
		flow_t total_flow = 0;
		cost_t total_cost = 0;
		while (build_path(s, t)) {
			cost_t cost = 0;
			flow_t cur_flow = flow_inf;
			for (int x = t; x != s; x = edges[eid[x]].from) {
				const int id = eid[x];
				cost += edges[id].cost;
				cur_flow = min(cur_flow, edges[id].cap - edges[id].flow);
			}
			if (cur_flow == 0) {
				break;
			}
			if (cost > 0) {
				break;
			}
			total_flow += cur_flow;
			total_cost += cur_flow * cost;
			for (int x = t; x != s; x = edges[eid[x]].from) {
				const int id = eid[x];
				edges[id].flow += cur_flow;
				edges[id ^ 1].flow -= cur_flow;
			}
		}
		return {total_flow, total_cost};
	}
};
