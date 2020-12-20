#pragma once

#include "dinic.h"

vector<int> dinic_matching(const vector<vector<int>>& ed) {
	int m = 0;
	for (const auto& v : ed) {
		if (!v.empty()) {
			m = max(m, *max_element(all(v)) + 1);
		}
	}
	const int n = ed.size();
	Dinic<int> net(n + m + 2);
	for (int i = 0; i < n; ++i) {
		net.add_edge(0, i + 1, 1);
		for (int j : ed[i]) {
			net.add_edge(i + 1, j + n + 1, 1);
		}
	}
	for (int i = 0; i < m; ++i) {
		net.add_edge(i + n + 1, m + n + 1, 1);
	}
	net.max_flow(0, n + m + 1);
	vector<int> ans(n, -1);
	for (const auto& e : net.edges) {
		if (e.flow == 1 && e.from <= n && e.to > n) {
			ans[e.from - 1] = e.to - n - 1;
		}
	}
	return ans;
}

vector<int> kuhn_matching(const vector<vector<int>>& ed) {
	int m = 0;
	for (const auto& v : ed) {
		if (!v.empty()) {
			m = max(m, *max_element(all(v)) + 1);
		}
	}
	const int n = ed.size();

	vector<int> rt(m, -1);
	vector<int> lt(n, -1);
	vector<int> used(n);
	int timer = 0;

	function<bool(int)> dfs = [&](int v) {
		if (used[v] == timer) {
			return false;
		}
		used[v] = timer;
		for (int x : ed[v]) {
			if (rt[x] == -1) {
				lt[v] = x;
				rt[x] = v;
				return true;
			}
		}
		for (int x : ed[v]) {
			if (dfs(rt[x])) {
				lt[v] = x;
				rt[x] = v;
				return true;
			}
		}
		return false;
	};

	for (int run = 1; run;) {
		run = 0;
		++timer;
		for (int i = 0; i < n; ++i) {
			if (lt[i] == -1 && dfs(i)) {
				run = 1;
			}
		}
	}

	return lt;
}

template <typename int_type>
vector<int> hungarian(const vector<vector<int_type>>& a) {
	const int n = a.size();
	constexpr int_type inf = numeric_limits<int_type>::max();

	vector<int_type> u(n + 1), v(n + 1);
	vector<int_type> est(n + 1, inf);
	vector<int> prv(n + 1, n);
	vector<char> used(n + 1);
	vector<int> rt(n + 1, n);

	for (int i = 0; i < n; ++i) {
		fill(all(est), inf);
		fill(all(used), 0);
		rt[n] = i;
		int r = n;
		do {
			used[r] = true;
			int l = rt[r];
			int nx = n;
			int_type delta = inf;
			for (int j = 0; j < n; ++j) {
				if (!used[j]) {
					int_type cur = a[l][j] - u[l] - v[j];
					if (cur < est[j]) {
						est[j] = cur;
						prv[j] = r;
					}
					if (est[j] < delta) {
						delta = est[j];
						nx = j;
					}
				}
			}
			for (int j = 0; j <= n; ++j) {
				if (used[j]) {
					u[rt[j]] += delta;
					v[j] -= delta;
				} else {
					est[j] -= delta;
				}
			}
			r = nx;
		} while (rt[r] != n);
		do {
			int pr = prv[r];
			rt[r] = rt[pr];
			r = pr;
		} while (r < n);
	}

	vector<int> ans(n);
	for (int i = 0; i < n; ++i) {
		ans[rt[i]] = i;
	}

	return ans;
}