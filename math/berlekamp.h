#pragma once

#include "../base/base.h"

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