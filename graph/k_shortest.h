#pragma once

#include "graph.h"
#include "../ds/persistent/heap.h"

template <>
vector<EdgeInt> Graph<Directed, Weighted>::k_shortest(int k, int from, int to) const {
	if (k == 0) {
		return {};
	}
	auto revd = reversed();
	auto [dst, par] = revd.get_weighted_distances_with_parents(to);
	static constexpr EdgeInt inf = numeric_limits<EdgeInt>::max();
	if (dst[from] == inf) {
		return {};
	}

	using Ph = PersistentHeap<pair<EdgeInt, int>>;

	vector<int> good_vertices = revd.get_last_order();
	assert(good_vertices[0] == to);

	vector<Ph*> heaps(n, nullptr);
	for (int v : good_vertices) {
		if (par[v] > -1) {
			heaps[v] = clone(heaps[par[v]]);
		}
		bool used_parent = false;
		for (int eid : a[v]) {
			const auto& e = edges[eid];
			if (dst[e.to] == inf) {
				continue;
			}
			if (!used_parent && e.to == par[v] && e.w + dst[e.to] == dst[v]) {
				used_parent = true;
			} else {
				heaps[v] = push(heaps[v], {e.w + dst[e.to] - dst[v], e.to});
			}
		}
	}

	using State = pair<EdgeInt, Ph*>;
	priority_queue<State, vector<State>, greater<State>> pq;
	vector<EdgeInt> res = {dst[from]};
	if (heaps[from]) {
		pq.push({dst[from] + heaps[from]->key.first, heaps[from]});
	}

	for (int it = 1; it < k && !pq.empty(); ++it) {
		auto [val, h] = pq.top();
		pq.pop();
		res.push_back(val);
		auto tmp = val - h->key.first;
		for (int i : {0, 1}) {
			if (!h->son[i]) {
				continue;
			}
			pq.push({tmp + h->son[i]->key.first, h->son[i]});
		}
		int v = h->key.second;
		if (heaps[v]) {
			pq.push({val + heaps[v]->key.first, heaps[v]});
		}
	}
	return res;
}