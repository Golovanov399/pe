#pragma once

#include <vector>
#include <utility>

using std::vector;

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
	SegTree() {}

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

	Node top() {
		return (int)a.size() > 1 ? a[1] : Node();
	}

	template <typename... Args>
	void update_point(int pos, Args&&... args) {
		{
			int v = 1, l = 0, r = n;
			while (v < n) {
				push(v);
				int m = (l + r) / 2;
				if (pos < m) {
					v *= 2;
					r = m;
				} else {
					v = v * 2 + 1;
					l = m;
				}
			}
		}
		pos += n;
		a[pos].update(std::forward<Args>(args)...);
		while (pos > 1) {
			pos /= 2;
			a[pos] = merge(a[pos * 2], a[pos * 2 + 1]);
		}
	}

	template <typename... Args>
	void update(int l, int r, Args&&... args) {
		_update(1, 0, n, l, r, std::forward<Args>(args)...);
	}

protected:
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

private:
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
			a[v].update(std::forward<Args>(args)...);
			return;
		}
		push(v);
		int m = (l1 + r1) / 2;
		_update(v + v, l1, m, l, r, std::forward<Args>(args)...);
		_update(v + v + 1, m, r1, l, r, std::forward<Args>(args)...);
		a[v] = merge(a[v + v], a[v + v + 1]);
	}
};