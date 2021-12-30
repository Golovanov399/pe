#pragma once

#include <vector>
#include <cassert>

using std::vector;

// TODO: add a concept
template <typename T, typename Func>
struct SlidingQueue {
	vector<T> head, tail;
	vector<T> fhead, ftail;
	Func func;

	size_t size() const {
		return head.size() + tail.size();
	}

	bool empty() const {
		return head.empty() && tail.empty();
	}

	void push(const T& x) {
		T nw = head.empty() ? x : func(fhead.back(), x);
		head.push_back(x);
		fhead.push_back(nw);
	}

	void pop() {
		if (tail.empty()) {
			transport();
		}
		tail.pop_back();
		ftail.pop_back();
	}

	void transport() {
		while (!head.empty()) {
			const auto& x = head.back();
			T nw = tail.empty() ? x : func(x, ftail.back());
			tail.push_back(x);
			ftail.push_back(nw);
			head.pop_back();
			fhead.pop_back();
		}
	}

	T get() const {
		if (!head.empty() && !tail.empty()) {
			return func(ftail.back(), fhead.back());
		} else if (!head.empty()) {
			return fhead.back();
		} else if (!tail.empty()) {
			return ftail.back();
		} else {
			assert(false);
		}
	}

	T operator ()() const {
		return get();
	}
};
