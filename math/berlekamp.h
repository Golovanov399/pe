#pragma once

#include <vector>

using std::vector;

template <typename T>
vector<T> find_linear_recurrence(const vector<T>& a) {
	vector<T> p;
	auto get = [&](int idx) -> T {
		T res = 0;
		for (int i = 0; i < (int)p.size(); ++i) {
			res += a[idx - 1 - i] * p[i];
		}
		return res;
	};
	vector<T> last_incorrect;
	int last_idx = -1;
	for (int i = 0; i < (int)a.size(); ++i) {
		auto cur = get(i);
		if (cur == a[i]) {
			continue;
		}
		auto new_p = p;
		bool need_to_upd = true;
		if (new_p.empty()) {
			new_p.assign(i + 1, 0);
		} else {
			const int sz = (i - last_idx - 1) + (int)last_incorrect.size();
			if (sz > (int)new_p.size()) {
				new_p.resize(sz);
			} else {
				need_to_upd = false;
			}
			for (int j = 0; j < (int)last_incorrect.size(); ++j) {
				new_p[i - last_idx - 1 + j] += last_incorrect[j] * (a[i] - cur);
			}
		}
		if (need_to_upd) {
			last_incorrect = p;
			last_incorrect.insert(last_incorrect.begin(), -1);
			auto inv = 1 / (cur - a[i]);
			for (auto& x : last_incorrect) {
				x *= inv;
			}
			last_idx = i;
		}
		p.swap(new_p);
	}
	return p;
}

template <typename T>
T get_nth_term_of_recurrence(const vector<T>& rec, const vector<T>& initial, long long n) {
	if (n < (int)initial.size()) {
		return initial[n];
	}
	const int sz = (int)rec.size();
	auto mult = [&](const vector<T>& a, const vector<T>& b) {
		vector<T> res(sz * 2);
		for (int i = 0; i < sz; ++i) {
			for (int j = 0; j < sz; ++j) {
				res[i + j] += a[i] * b[j];
			}
		}
		for (int i = 2 * sz - 1; i >= sz; --i) {
			if (!res[i]) {
				continue;
			}
			for (int j = 0; j < sz; ++j) {
				res[i - 1 - j] += res[i] * rec[j];
			}
		}
		res.resize(sz);
		return res;
	};

	if (sz == 1) {
		T answer = initial[0];
		T cur = rec[0];
		while (n) {
			if (n & 1ll) {
				answer *= cur;
			}
			n >>= 1;
			cur *= cur;
		}
		return answer;
	}

	vector<T> res(sz);
	res[0] = 1;
	vector<T> a(sz);
	a[1] = 1;
	while (n) {
		if (n & 1ll) {
			res = mult(res, a);
		}
		n >>= 1;
		a = mult(a, a);
	}
	T answer = 0;
	for (int i = 0; i < sz; ++i) {
		answer += res[i] * initial[i];
	}
	return answer;
}

template <typename T>
T guess_nth_term(const vector<T>& a, long long n) {
	auto rec = find_linear_recurrence(a);
	return get_nth_term_of_recurrence(rec, a, n);
}
