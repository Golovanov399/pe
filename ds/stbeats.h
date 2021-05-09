#pragma once

#include "segtree.h"

/*
	Node struct must have:
	- Node::Node()
	- Node::Node(const T&)
	- void Node::push(Node&)
	- void Node::clear_after_push()	
	- void Node::update(Args...)
	- Node merge(const Node&, const Node&)

	- bool Node::tag_condition(Args...)
	- bool Node::break_condition(Args...)
*/
template <typename Node>
class SegTreeBeats : public SegTree<Node> {
public:
	using SegTree<Node>::SegTree;

	template <typename... Args>
	void update(int l, int r, Args&&... args) {
		_update(1, 0, this->n, l, r, forward<Args>(args)...);
	}

private:
	template <typename... Args>
	void _update(int v, int l1, int r1, int l, int r, Args&&... args) {
		if (l >= r1 || l1 >= r) {
			return;
		}
		if (l <= l1 && r >= r1) {
			if (this->a[v].break_condition(forward<Args>(args)...)) {
				return;
			} else if (this->a[v].tag_condition(forward<Args>(args)...)) {
				this->a[v].update(forward<Args>(args)...);
				return;
			}
		}
		this->push(v);
		int m = (l1 + r1) / 2;
		_update(v + v, l1, m, l, r, forward<Args>(args)...);
		_update(v + v + 1, m, r1, l, r, forward<Args>(args)...);
		this->a[v] = merge(this->a[v + v], this->a[v + v + 1]);
	}
};

template <typename Node>
using StBeats = SegTreeBeats<Node>;