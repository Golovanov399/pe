#pragma once

#include "../base/base.h"

/*
	Node struct must have:
	- Node::Node()
	- Node::Node(const T&)
	- void Node::push(Node&)
	- void Node::clear_after_push()	
	- void Node::update(Args...)
	- Node merge(const Node&, const Node&)
*/
template <typename Node>
class SegTree {
public:
	explicit SegTree(int _n) {
		n = 1;
		while (n < _n) {
			n *= 2;
		}
		a.resize(n + n);
		for (int i = n - 1; i > 0; --i) {
			a[i] = merge(a[i + i], a[i + i + 1]);
		}
	}

	template <typename T>
	explicit SegTree(const vector<T>& ar) {
		n = 1;
		while (n < (int)ar.size()) {
			n *= 2;
		}
		a.resize(n + n);
		for (int i = 0; i < (int)ar.size(); ++i) {
			a[n + i] = Node(ar[i]);
		}
		for (int i = n - 1; i > 0; --i) {
			a[i] = merge(a[i + i], a[i + i + 1]);
		}
	}

	Node get(int l, int r) {
		return get(1, 0, n, l, r);
	}

	template <typename... Args>
	void update_point(int pos, Args&&... args) {
		pos += n;
		a[pos].update(forward<Args>(args)...);
		while (pos > 1) {
			pos /= 2;
			a[pos] = merge(a[pos * 2], a[pos * 2 + 1]);
		}
	}

	template <typename... Args>
	void update(int l, int r, Args&&... args) {
		_update(1, 0, n, l, r, forward<Args>(args)...);
	}

private:
	int n;
	vector<Node> a;

	void push(int v) {
		if (v + v < n + n) {
			for (int t : {0, 1}) {
				a[v].push(a[v + v + t]);
			}
			a[v].clear_after_push();
		}
	}

	Node get(int v, int l1, int r1, int l, int r) {
		if (l >= r1 || l1 >= r) {
			return Node{};
		}
		if (l <= l1 && r >= r1) {
			return a[v];
		}
		push(v);
		int m = (l1 + r1) / 2;
		return merge(get(v + v, l1, m, l, r), get(v + v + 1, m, r1, l, r));
	}

	template <typename... Args>
	void _update(int v, int l1, int r1, int l, int r, Args&&... args) {
		if (l >= r1 || l1 >= r) {
			return;
		}
		if (l <= l1 && r >= r1) {
			a[v].update(forward<Args>(args)...);
			return;
		}
		push(v);
		int m = (l1 + r1) / 2;
		_update(v + v, l1, m, l, r, forward<Args>(args)...);
		_update(v + v + 1, m, r1, l, r, forward<Args>(args)...);
		a[v] = merge(a[v + v], a[v + v + 1]);
	}
};