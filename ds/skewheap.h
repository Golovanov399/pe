#pragma once

#include "../base/base.h"

template <typename T>
struct SkewHeap {
	struct alignas(32) Node {
		T key;
		int p, l, r;
		Node(const T& _key = T()): key(_key), p(-1), l(-1), r(-1) {}
	};

	vector<Node> v;
	int root;

	SkewHeap(int n): v(n), root(-1) {}

	int meld(int x, int y) {
		if (x == -1 || y == -1) {
			return x == -1 ? y : x;
		}
		if (v[x].key > v[y].key) {
			swap(x, y);
		}
		v[x].r = meld(v[x].r, y);
		v[v[x].r].p = x;
		swap(v[x].l, v[x].r);
		return x;
	}

	void pop() {
		root = meld(v[root].l, v[root].r);
	}

	void update(int idx, const T& key) {
		Node& node = v[idx];
		node.key = key;
		if (idx == root) {
			return;
		}
		Node& par = v[node.p];
		if (par.key <= key) {
			return;
		}
		(par.l == idx ? par.l : par.r) = -1;
		node.p = -1;
		root = meld(root, idx);
	}

	bool empty() const {
		return root == -1;
	}

	T top() const {
		return v[root].key;
	}

	T& operator [](int idx) {
		return v[idx].key;
	}

	const T& operator [](int idx) const {
		return v[idx].key;
	}
};