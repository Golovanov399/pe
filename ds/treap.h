#pragma once

#include "../base/base.h"
#include "../base/random.h"
#include "../base/memory.h"

template <typename T, typename MemMgr = DefaultMemoryManager>
struct Treap {
	struct Node : public MemMgr {
		Node *left, *right;
		T key;
		int prior;
		int size;
		bool rev;

		Node(const T& _key): left(nullptr), right(nullptr), key(_key), prior(rand()), size(1), rev(false) {}
	};

	int get_size(Node* node) {
		return node ? node->size : 0;
	}

	void push(Node* node) {
		if (!node) {
			return;
		}
		if (node->rev) {
			swap(node->left, node->right);
			node->rev = false;
			if (node->left) {
				node->left->rev ^= 1;
			}
			if (node->right) {
				node->right->rev ^= 1;
			}
		}
	}

	void recalc(Node* node) {
		node->size = 1 + get_size(node->left) + get_size(node->right);
	}

	void split_by_key(Node* v, Node*& l, Node*& r, const T& key) {
		if (!v) {
			l = r = nullptr;
			return;
		}
		push(v);
		if (v->key <= key) {
			split_by_key(v->right, v->right, r, key);
			l = v;
		} else {
			split_by_key(v->left, l, v->left, key);
			r = v;
		}
		recalc(v);
	}

	void split_by_size(Node* v, Node*& l, Node*& r, int size) {
		if (!v) {
			l = r = nullptr;
			return;
		}
		push(v);
		if (get_size(v->left) < size) {
			split_by_size(v->right, v->right, r, size - 1 - get_size(v->left));
			l = v;
		} else {
			split_by_size(v->left, l, v->left, size);
			r = v;
		}
		recalc(v);
	}

	Node* merge(Node* l, Node* r) {
		if (!l) {
			return r;
		}
		if (!r) {
			return l;
		}
		if (l->prior < r->prior) {
			push(l);
			l->right = merge(l->right, r);
			recalc(l);
			return l;
		} else {
			push(r);
			r->left = merge(l, r->left);
			recalc(r);
			return r;
		}
	}

	Node* root;

	Treap(): root(nullptr) {}

	void add(const T& key) {
		root = merge(root, new Node(key));
	}

	void push_back(const T& key) {
		add(key);
	}

	void insert(int pos, const T& key) {
		Node *l, *r;
		split_by_size(root, l, r, pos);
		root = merge(l, merge(new Node(key), r));
	}

	void insert_by_key(const T& key) {
		Node *l, *r;
		split_by_key(root, l, r, key);
		root = merge(l, merge(new Node(key), r));
	}

	void append(Node* node) {
		root = merge(root, node);
	}

	void insert(int pos, Node* node) {
		Node *l, *r;
		split_by_size(root, l, r, pos);
		root = merge(l, merge(node, r));
	}

	void insert_by_key(Node* node) {
		Node *l, *r;
		split_by_key(root, l, r, node->key);
		root = merge(l, merge(node, r));
	}

	void erase_by_pos(int pos) {
		Node *l, *r;
		split_by_size(root, l, r, pos);
		split_by_size(r, root, r, 1);
		root = merge(l, r);
	}

	void erase_by_key(const T& key) {
		if (!find(key)) {
			return;
		}
		Node *l, *r;
		split_by_key(root, l, r, key);
		split_by_size(l, l, root, get_size(l) - 1);
		root = merge(l, r);
	}

	Node* find(const T& key) {
		Node* node = root;
		while (node) {
			push(node);
			if (node->key == key) {
				return node;
			} else if (node->key < key) {
				node = node->right;
			} else {
				node = node->left;
			}
		}
		return nullptr;
	}

	void reverse(int l, int r) {
		assert(l <= r);
		if (l == r) {
			return;
		}
		Node *left, *mid, *right;
		split_by_size(root, mid, right, r);
		split_by_size(mid, left, mid, l);
		mid->rev ^= 1;
		root = merge(left, merge(mid, right));
	}

	Node* cut_out(int l, int r) {
		assert(l <= r);
		if (l == r) {
			return nullptr;
		}
		Node *left, *mid, *right;
		split_by_size(root, mid, right, r);
		split_by_size(mid, left, mid, l);
		root = merge(left, right);
		return mid;
	}

	void traverse(Node* node, const function<void(const T&)>& f) {
		if (!node) {
			return;
		}
		push(node);
		traverse(node->left, f);
		f(node->key);
		traverse(node->right, f);
	}

	vector<T> get_all() {
		vector<T> res;
		traverse(root, [&](const T& key) {
			res.push_back(key);
		});
		return res;
	}
};