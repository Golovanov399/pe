#pragma once

#include "../base/base.h"

template <size_t alphabet_size = 26>
class SuffixAutomaton {
public:
	struct Node {
		array<int, alphabet_size> to;
		int link;
		int len;
		bool term;

		explicit Node(const array<int, alphabet_size>& _to = {}, int _link = -1, int _len = 0, bool _term = false):
				to(_to),
				link(_link),
				len(_len),
				term(_term) {}
	};
	
	SuffixAutomaton(): nodes({Node()}), last(0) {
		nodes[0].to.fill(-1);
	}

	explicit SuffixAutomaton(const string& s): nodes({Node()}), last(0) {
		nodes[0].to.fill(-1);
		build(s);
	}

	void build(const string& s) {
		for (char c : s) {
			add(c - 'a');
		}
		mark_terminal();
	}

	explicit SuffixAutomaton(const vector<int>& vec): nodes({Node()}), last(0) {
		nodes[0].to.fill(-1);
		build(vec);
	}

	void build(const vector<int>& vec) {
		for (int x : vec) {
			add(x);
		}
		mark_terminal();
		vector<int> perm(nodes.size());
	}

	void add(int c) {
		int v = last;
		int nn = new_node(array<int, alphabet_size>{}, -1, nodes[last].len + 1, false);
		last = nn;
		while (v > -1 && nodes[v].to[c] == -1) {
			nodes[v].to[c] = nn;
			v = nodes[v].link;
		}
		if (v == -1) {
			nodes[nn].link = 0;
			return;
		}
		int q = nodes[v].to[c];
		if (nodes[v].len + 1 == nodes[q].len) {
			nodes[nn].link = q;
			return;
		}
		int clone = new_node(nodes[q]);
		nodes[clone].len = nodes[v].len + 1;
		nodes[q].link = clone;
		nodes[nn].link = clone;
		for (int u = v; u >= 0 && nodes[u].to[c] == q; u = nodes[u].link) {
			nodes[u].to[c] = clone;
		}
	}

	const Node& operator [](int v) const {
		return nodes[v];
	}

	int maxlen(int v) const {
		return nodes[v].len;
	}

	int minlen(int v) const {
		return v ? nodes[nodes[v].link].len + 1 : 0;
	}

	int size() const {
		return nodes.size();
	}

	int go(int v, int c) const {
		return nodes[v].to[c];
	}

	int go(int v, const string& s) const {
		for (char c : s) {
			if (v == -1) {
				return -1;
			}
			v = go(v, c - 'a');
		}
		return v;
	}

	int go(int v, const vector<int>& vec) const {
		for (char x : vec) {
			if (v == -1) {
				return -1;
			}
			v = go(v, x);
		}
		return v;
	}

	int proceed(int v, int c) const {
		while (v >= 0 && nodes[v].to[c] == -1) {
			v = nodes[v].link;
		}
		if (v == -1) {
			return 0;
		} else {
			return nodes[v].to[c];
		}
	}

	int proceed(int v, const string& s) const {
		for (char c : s) {
			v = proceed(v, c - 'a');
		}
		return v;
	}

	int proceed(int v, vector<int>& vec) const {
		for (char x : vec) {
			v = proceed(v, x);
		}
		return v;
	}

	bool matches(const string& s) {
		int v = 0;
		for (char c : s) {
			v = proceed(v, c - 'a');
			if (v == last) {
				return true;
			}
		}
		return false;
	}

	bool matches(const vector<int>& vec) {
		int v = 0;
		for (char x : vec) {
			v = proceed(v, x);
			if (v == last) {
				return true;
			}
		}
		return false;
	}

private:
	vector<Node> nodes;
	int last;

	template <typename... Args>
	int new_node(Args... args) {
		int res = nodes.size();
		nodes.push_back(Node{args...});
		nodes.back().to.fill(-1);
		return res;
	}

	int new_node(const Node& node) {
		int res = nodes.size();
		nodes.push_back(node);
		return res;
	}

	void mark_terminal() {
		for (int v = last; v >= 0; v = nodes[v].link) {
			nodes[v].term = true;
		}
	}
};