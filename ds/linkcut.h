#pragma once

#include "splay.h"

template <typename T, typename AggrF = Nop, typename UpdateF = Nop, typename AfterUpdF = Nop>
struct LinkCutRooted : SplayTree<T, AggrF, UpdateF, AfterUpdF> {
	// now node->par is always either normal parent or path parent
	// we don't know which because we never care?

	using Super = SplayTree<T, AggrF, UpdateF, AfterUpdF>;
	using Node = typename Super::Node;

	LinkCutRooted(int n): Super(n) {
		for (int i = 0; i < n; ++i) {
			this->new_node();
		}
	}

	using Super::splay, Super::make_son, Super::recalc, Super::leftmost, Super::get_root, Super::nodes;

	Node* expose(Node* node) {	// returns last met vertex, needed for lca
		Node *v = node, *last = nullptr;
		while (v) {
			splay(v);
			make_son(v, 1, last);
			recalc(v);
			last = v;
			v = v->par;
		}
		splay(node);
		return last;
	}

	void link(Node* u, Node* v) {
		expose(v);
		expose(u);
		make_son(u, 1, v);
		recalc(u);
	}

	void cut(Node* u, Node* v) {
		expose(u);
		expose(v);
		make_son(v, 0, nullptr);
		u->par = nullptr;
		recalc(v);
	}

	Node* get_tree_root(Node* v) {
		expose(v);
		return leftmost(v);
	}

	Node* lca(Node* u, Node* v) {
		if (u == v) {
			return u;
		}
		expose(u);
		auto l = expose(v);
		return u->par ? l : nullptr;
	}

	void link(int u, int v) {
		link(nodes + u, nodes + v);
	}

	void cut(int u, int v) {
		cut(nodes + u, nodes + v);
	}

	int get_tree_root(int v) {
		return get_tree_root(nodes + v) - nodes;
	}

	int lca(int u, int v) {
		return lca(nodes + u, nodes + v) - nodes;
	}
};

template <typename T, typename AggrF = Nop, typename UpdateF = Nop, typename AfterUpdF = Nop>
struct LinkCut : SplayTree<T, AggrF, UpdateF, AfterUpdF> {
	// now node->par is always either normal parent or path parent
	// we don't know which because we never care?

	using Super = SplayTree<T, AggrF, UpdateF, AfterUpdF>;
	using Node = typename Super::Node;

	LinkCut(int n): Super(n) {
		for (int i = 0; i < n; ++i) {
			this->new_node();
		}
	}

	using Super::splay, Super::make_son, Super::recalc, Super::leftmost, Super::get_root, Super::nodes;

	Node* expose(Node* node) {	// returns last met vertex, needed for lca
		Node *v = node, *last = nullptr;
		while (v) {
			splay(v);
			make_son(v, 1, last);
			recalc(v);
			last = v;
			v = v->par;
		}
		splay(node);
		return last;
	}

	void reroot(Node* node) {
		expose(node);
		node->rev ^= 1;
	}

	void link(Node* u, Node* v) {
		reroot(v);
		expose(u);
		make_son(u, 1, v);
		recalc(u);
	}

	void cut(Node* u, Node* v) {
		reroot(u);
		expose(v);
		make_son(v, 0, nullptr);
		u->par = nullptr;
		recalc(v);
	}

	Node* get_tree_root(Node* v) {
		expose(v);
		return leftmost(v);
	}

	Node* lca(Node* u, Node* v) {
		if (u == v) {
			return u;
		}
		expose(u);
		auto l = expose(v);
		return u->par ? l : nullptr;
	}

	void link(int u, int v) {
		link(nodes + u, nodes + v);
	}

	void cut(int u, int v) {
		cut(nodes + u, nodes + v);
	}

	int get_tree_root(int v) {
		return get_tree_root(nodes + v) - nodes;
	}

	int lca(int u, int v) {
		return lca(nodes + u, nodes + v) - nodes;
	}
};