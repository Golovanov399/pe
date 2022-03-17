#pragma once

#include <algorithm>
#include <cstring>
#include <iostream>

#include "../base/functors.h"

using std::is_void_v, std::enable_if_t;
using std::swap, std::cerr;

template <typename S>
struct SplayNode {
	SplayNode<S>* child[2];	// l, r
	SplayNode<S>* par;
	int sz;
	bool rev;

	S value;

	SplayNode(): par(nullptr), sz(0), rev(false), value() {
		memset(child, 0, sizeof(child));
	}

	explicit SplayNode(const S& val): par(nullptr), sz(1), rev(false), value(val) {
		memset(child, 0, sizeof(child));
	}

	inline bool is_root() const {
		return parent_index() == -1;
	}

	int parent_index() const {
		if (!par) {
			return -1;
		}
		return par->child[0] == this ? 0 : par->child[1] == this ? 1 : -1;
	}
};

template <>
struct SplayNode<void> {
	SplayNode<void>* child[2];	// l, r
	SplayNode<void>* par;
	int sz;
	bool rev;

	SplayNode(): par(nullptr), sz(0), rev(false) {
		memset(child, 0, sizeof(child));
	}

	inline bool is_root() const {
		return parent_index() == -1;
	}

	int parent_index() const {
		if (!par) {
			return -1;
		}
		return par->child[0] == this ? 0 : par->child[1] == this ? 1 : -1;
	}
};

template <typename T, typename AggrF = Nop, typename UpdateF = Nop, typename AfterUpdF = Nop>
struct SplayTree {
	using Node = SplayNode<T>;
	Node *nodes;
	int sz;

	AggrF aggr;
	UpdateF upd;
	AfterUpdF after_upd;

	SplayTree(int cap): sz(0) {
		nodes = (Node*)malloc(sizeof(Node) * cap);
	}
	~SplayTree() {
		delete[] nodes;
	}
	void clear() {
		sz = 0;
	}

	template <typename... Types>
	Node* new_node(Types... args) {
		return &(nodes[sz++] = Node(args...));
	}

	void push(Node* node) {
		if (!node) {
			return;
		}
		for (int i : {0, 1}) {
			if (node->child[i]) {
				node->child[i]->rev ^= node->rev;
				if constexpr (!is_void_v<T>) {
					upd(node->child[i]->value, node->value);
				}
			}
		}
		if (node->rev) {
			swap(node->child[0], node->child[1]);
			node->rev = false;
		}
		if constexpr (!is_void_v<T>) {
			after_upd(node->value);
		}
	}

	int get_size(Node* node) {
		return node ? node->sz : 0;
	}

	void recalc(Node* node) {
		if (!node) {
			return;
		}
		node->sz = get_size(node->child[0]) + 1 + get_size(node->child[1]);
		if constexpr (!is_void_v<T>) {
			node->value = aggr(node->child[0] ? node->child[0]->value : T(), node->value, node->child[1] ? node->child[1]->value : T());
		}
	}

	void make_son(Node* par, int id, Node* son) {
		if (id == -1) {
			if (son) {
				son->par = par;
			}
			return;
		}
		par->child[id] = son;
		if (son) {
			son->par = par;
		}
	}

	void rotate(Node* son) { // son becomes par's parent
		Node *par = son->par;
		int i = son->parent_index();
		int j = par->parent_index();
		make_son(par->par, j, son);
		make_son(par, i, son->child[!i]);
		make_son(son, !i, par);
		recalc(par);
		recalc(son);
	}

	void splay(Node* node) {
		while (!node->is_root()) {
			auto p = node->par;
			auto pp = p->par;
			push(pp);
			push(p);
			push(node);
			if (!p->is_root()) {
				if (node->parent_index() == p->parent_index()) {
					rotate(p);
				} else {
					rotate(node);
				}
			}
			rotate(node);
			recalc(pp);
			recalc(p);
			recalc(node);
		}
		push(node);
	}

	Node* get_root(Node* node) {
		if (!node) {
			return nullptr;
		}
		while (~node->parent_index()) {
			node = node->par;
		}
		return node;
	}

	Node* leftmost(Node* node) {
		if (!node) {
			return nullptr;
		}
		while (push(node), node->child[0]) {
			node = node->child[0];
		}
		return node;
	}

	Node* rightmost(Node* node) {
		if (!node) {
			return nullptr;
		}
		while (push(node), node->child[1]) {
			node = node->child[1];
		}
		return node;
	}

	Node* get_kth(Node* root, int k) {
		push(root);
		if (int lsz = get_size(root->child[0]); lsz > k) {
			return get_kth(root->child[0], k);
		} else if (lsz == k) {
			return root;
		} else {
			return get_kth(root->child[1], k - lsz - 1);
		}
	}

	Node* cut_after(Node* node) {	// returns the right hand subtree
		assert(node);
		splay(node);
		auto res = node->child[1];
		if (node->child[1]) {
			node->child[1]->par = nullptr;
			node->child[1] = nullptr;
			recalc(node);
		}
		return res;
	}

	Node* cut_before(Node* node) {	// returns the left hand subtree
		assert(node);
		splay(node);
		auto res = node->child[0];
		if (node->child[0]) {
			node->child[0]->par = nullptr;
			node->child[0] = nullptr;
			recalc(node);
		}
		return res;
	}

	Node* merge(Node* left, Node* right) {
		if (!left) {
			return right;
		}
		if (!right) {
			return left;
		}
		Node* root = rightmost(left);
		splay(root);
		root->child[1] = right;
		right->par = root;
		recalc(root);
		return root;
	}

	void reverse(Node* from, Node* to) {
		auto l = cut_before(from);
		auto r = cut_after(to);
		to->rev ^= 1;
		merge(merge(l, to), r);
	}

	template <typename S = T>
	enable_if_t<!is_void_v<T>, S> aggregate(Node* from, Node* to) {
		auto l = cut_before(from);
		auto r = cut_after(to);
		T res = to->value;
		merge(merge(l, to), r);
		return res;
	}

	void reverse(int l, int r) {	// we assume that they all have the same root
		Node *root = get_root(nodes);
		Node *from = get_kth(root, l), *to = get_kth(root, r - 1);
		reverse(from, to);
	}

	template <typename S = T>
	enable_if_t<!is_void_v<T>, S> aggregate(int l, int r) {	// we assume that they all have the same root
		Node *root = get_root(nodes);
		Node *from = get_kth(root, l), *to = get_kth(root, r - 1);
		return aggregate(from, to);
	}

	void cerr_all(Node* root) {
		if (!root) {
			return;
		}
		cerr_all(root->child[0]);
		cerr << (root - nodes) << " ";
		cerr_all(root->child[1]);
	}

	void show_all(Node* root) {
		cerr_all(root);
		cerr << "\n";
	}
};

