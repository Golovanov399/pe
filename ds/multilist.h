#pragma once

#include "../base/base.h"

template <typename T>
class Multilist {
public:
	Multilist(int count, int capacity): head(count, -1), next(capacity, -1), data(capacity), sz(0) {}

	class Iterator : public iterator<forward_iterator_tag, T, int> {
	public:
		using base = iterator<forward_iterator_tag, T, int>;
		using typename base::reference;
		Iterator(Multilist* _ml, int _idx): ml(_ml), idx(_idx) {}
		Iterator& operator ++() {
			assert(~idx);
			idx = ml->next[idx];
			return *this;
		}
		Iterator operator ++(int) {
			assert(~idx);
			return Iterator(ml, ml->next[idx]);
		}
		bool operator ==(const Iterator& ot) const {
			return ml == ot.ml && idx == ot.idx;
		}
		bool operator !=(const Iterator& ot) const {
			return ml != ot.ml || idx != ot.idx;
		}
		reference operator *() const {
			return ml->data[idx];
		}

	private:
		Multilist* ml;
		int idx;
	};

	class InnerContainer {
	public:
		InnerContainer(Multilist* _owner, int _idx): owner(_owner), idx(_idx) {}

		bool empty() const {
			return owner->head[idx] == -1;
		}

		Iterator begin() const {
			return Iterator{owner, owner->head[idx]};
		}

		Iterator end() const {
			return Iterator{owner, -1};
		}

		void push_back(const T& x) {
			owner->data[owner->sz] = x;
			owner->next[owner->sz] = owner->head[idx];
			owner->head[idx] = owner->sz++;
		}

		void clear() {
			// does not free memory in any way
			owner->head[idx] = -1;
		}

	private:
		Multilist* owner;
		int idx;
	};

	InnerContainer operator [](int idx) {
		return InnerContainer{this, idx};
	}

	void drop_size() {
		// does not set head to -1
		sz = 0;
	}

	void clear() {
		sz = 0;
		fill(all(head), -1);
	}

private:
	vector<int> head;
	vector<int> next;
	vector<T> data;
	int sz;
};
