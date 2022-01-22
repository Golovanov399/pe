#pragma once

#include <vector>
#include <numeric>
#include <algorithm>

#include "../base/istream.h"
#include "../base/defines.h"

using std::vector;
using std::iota, std::swap;

class Tree {
public:
	explicit Tree(int _n): n(_n), g(_n) {}

	void add_edge(int u, int v) {
		g[u].push_back(v);
		g[v].push_back(u);
	}

	void read_edges(int start_index = 1) {
		for (int i = 0; i < n - 1; ++i) {
			int u = nxt() - start_index, v = nxt() - start_index;
			g[u].push_back(v);
			g[v].push_back(u);
		}
		set_root(0);
	}

	void read_n_parents(int start_index = 1) {
		par.assign(n, -1);
		for (int i = 0; i < n; ++i) {
			par[i] = nxt() - start_index;
			if (par[i] < start_index || par[i] >= start_index + n) {
				par[i] = -1;
				root = i;
			} else {
				g[par[i]].push_back(i);
			}
		}
	}

	void read_parents(int start_index = 1) {
		par.assign(n, -1);
		root = 0;
		for (int i = 1; i < n; ++i) {
			par[i] = nxt() - start_index;
			g[par[i]].push_back(i);
		}
	}

	void set_root(int v) {
		root = v;
		par.assign(n, -1);
		dfs_down(root, -1, [&](int v, int p) { par[v] = p; });
	}

	template <typename T, typename Func>
	vector<T> calc_dp(const Func& f) {
		// calculate dp[v] = f({dp[x] : x is a son of v})
		vector<T> ans(n);
		dfs_f(root, -1, f, ans);
		return ans;
	}

	template <typename T, typename Func>
	vector<T> calc_relaxing_dp(T def, const Func& f) {
		// calculate dp[v] = f(son1, f(son2, f(...(f(sonk, def)))))
		vector<T> ans(n);
		dfs_f(root, -1, [&](const vector<T>& vec) {
			T res = def;
			for (const auto& x : vec) {
				res = f(res, x);
			}
			return res;
		}, ans);
		return ans;
	}

	vector<int> tin, tout;
	vector<int> sz;
	vector<int> level;
	vector<int> par;
	vector<int> bps;	// binary parents aligned in 1d
	vector<int> hld_start;

	void fill_sizes() {
		sz.resize(n);
		run_dfs([&](int v, int) {
			sz[v] = 1;
		}, [&](int v, int p) {
			if (p > -1) {
				sz[p] += sz[v];
			}
		});
	}

	void fill_times() {
		tin.resize(n);
		tout.resize(n);
		int timer = 0;
		run_dfs([&](int v, int) {
			tin[v] = timer++;
		}, [&](int v, int) {
			tout[v] = timer;
		});
	}

	void fill_levels() {
		level.assign(n, 0);
		run_dfs([&](int v, int p) {
			if (p > -1) {
				level[v] = level[p] + 1;
			}
		}, [](...) {});
	}

	void fill_binary_parents() {
		const int l = 33 - __builtin_clz(n);
		bps.assign(n * l, -1);
		run_dfs([&](int v, int p) {
			int i = 0;
			while (p > -1) {
				// binpars[v].push_back(p);
				bps[v * l + i] = p;
				// if (i < (int)binpars[p].size()) {
				if (bps[p * l + i] > -1) {
					// p = binpars[p][i++];
					p = bps[p * l + i++];
				} else {
					p = -1;
				}
			}
		}, [](...) {});
	}

	void hld_sort() {
		fill_sizes();
		hld_start.resize(n);
		iota(all(hld_start), 0);
		run_dfs([&](int v, int p) {
			if (auto it = find(all(g[v]), p); it != g[v].end()) {
				g[v].erase(it);
			}
			sort(all(g[v]), [&](int x, int y) {
				return sz[x] > sz[y];
			});
			if (!g[v].empty()) {
				hld_start[g[v][0]] = hld_start[v];
			}
		}, [](...){});
		fill_times();
	}

	template <typename PreFunc, typename PostFunc>
	void run_dfs(const PreFunc& pre, const PostFunc& post) {
		dfs(root, -1, pre, post);
	}

	// some statistics

	int get_kth_parent(int v, int k) {
		const int l = 33 - __builtin_clz(n);
		while (k) {
			const int b = 31 - __builtin_clz(k);
			k ^= 1 << b;
			// if (b >= (int)binpars[v].size()) {
			if (bps[v * l + b] == -1) {
				return -1;
			} else {
				// v = binpars[v][b];
				v = bps[v * l + b];
			}
		}
		return v;
	}

	bool is_par(int u, int v) {
		return tin[u] <= tin[v] && tout[u] >= tout[v];
	}

	int lca(int u, int v) {
		if (is_par(v, u)) {
			return v;
		}
		const int l = 33 - __builtin_clz(n);
		for (int i = l - 1; i >= 0; --i) {
			if (bps[v * l + i] >= 0 && !is_par(bps[v * l + i], u)) {
				v = bps[v * l + i];
			}
		}
		return bps[v * l];
	}

	template <typename T, typename FuncSeg, typename F>
	T path_query(int u, int v, const FuncSeg& fseg, const F& f) {
		// f must be commutative and associative because im lazy
		T ans;
		bool first_time = true;
		while (!is_par(hld_start[u], v)) {
			auto tmp = fseg(tin[hld_start[u]], tin[u] + 1);
			ans = first_time ? tmp : f(ans, tmp);
			u = par[hld_start[u]];
			first_time = false;
		}
		while (hld_start[v] != hld_start[u]) {
			auto tmp = fseg(tin[hld_start[v]], tin[v] + 1);
			ans = first_time ? tmp : f(ans, tmp);
			v = par[hld_start[v]];
			first_time = false;
		}
		if (tin[u] > tin[v]) {
			swap(u, v);
		}
		auto tmp = fseg(tin[u], tin[v] + 1);
		ans = first_time ? tmp : f(ans, tmp);
		return ans;
	}

private:
	const int n;
	vector<vector<int>> g;
	int root;

	template <typename T, typename Func>
	void dfs_f(int v, int p, const Func& f, vector<T>& dp) {
		vector<T> tmp;
		for (int x : g[v]) {
			if (x != p) {
				dfs_f(x, v, f, dp);
				tmp.push_back(dp[x]);
			}
		}
		dp[v] = f(tmp);
	}

	template <typename Func>
	void dfs_down(int v, int p, const Func& f) {
		f(v, p);
		for (int x : g[v]) {
			if (x != p) {
				dfs_down(x, v, f);
			}
		}
	}

	template <typename PreFunc, typename PostFunc>
	void dfs(int v, int p, const PreFunc& pre, const PostFunc& post) {
		pre(v, p);
		for (int x : g[v]) {
			if (x != p) {
				dfs(x, v, pre, post);
			}
		}
		post(v, p);
	}
};