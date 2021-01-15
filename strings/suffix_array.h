#pragma once

#include "../ds/sparse.h"
#include "../ds/multilist.h"

class SuffixArray {
public:
	explicit SuffixArray(const string& _s, bool need_sparse = true): s(_s + '#'), n((int)_s.length() + 1) {
		vector<int> cur(n);
		for (int i = 0; i < n; ++i) {
			cur[i] = s[i];
		}
		suffix_order.resize(n);
		iota(all(suffix_order), 0);
		sort(all(suffix_order), [&](int i, int j) {
			return cur[i] < cur[j];
		});
		vector<int> ncur(n);
		Multilist<int> buckets(max(256, n), n);
		vector<int> used;
		for (int k = 1; k < n; k *= 2) {
			int val = 0;
			int last_pos = 0;

			auto refill_buckets = [&]() {
				sort(all(used));
				for (int p : used) {
					for (int i : buckets[p]) {
						suffix_order[last_pos++] = i;
						ncur[i] = val;
					}
					buckets[p].clear();
					++val;
				}
				used.clear();
				buckets.drop_size();
			};

			for (int i = 0; i < n; ++i) {
				if (i > 0 && cur[suffix_order[i]] > cur[suffix_order[i - 1]]) {
					refill_buckets();
				}
				int j = suffix_order[i];
				int b = cur[(j + k) % n];
				if (buckets[b].empty()) {
					used.push_back(b);
				}
				buckets[b].push_back(j);
			}
			refill_buckets();
			cur.swap(ncur);
		}
		if (need_sparse) {
			build_sparse();
		}
	}

	Sparse<int, Min<int>> build_sparse() {
		vector<int> lengths(n);
		vector<int> pos(n);
		for (int i = 0; i < n; ++i) {
			pos[suffix_order[i]] = i;
		}
		for (int i = 0, cur = 0; i < n; ++i) {
			if (cur > 0) {
				--cur;
			}
			if (pos[i] == n - 1) {
				lengths[n - 1] = -1;
				cur = 0;
				continue;
			}
			int j = suffix_order[pos[i] + 1];
			while (i + cur < n && j + cur < n && s[i + cur] == s[j + cur]) {
				++cur;
			}
			lengths[pos[i]] = cur;
		}
		common_lengths = decltype(common_lengths){lengths};
		return common_lengths;
	}

	vector<int> get_suffix_order() const {
		auto res = suffix_order;
		assert(res[0] == n - 1);
		res.erase(res.begin());
		return res;
	}

private:
	string s;
	int n;

	vector<int> suffix_order;
	Sparse<int, Min<int>> common_lengths;
};