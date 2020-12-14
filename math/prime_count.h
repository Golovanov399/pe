#pragma once

#include "sieve.h"
#include "../base/util.h"
#include "../ds/fenwick.h"

class PrimeCount {
public:
	const li n;
	const li K;

	static constexpr li def = numeric_limits<li>::min();

	vector<int> erat, primes;
	vector<int> sorted_numbers;
	vector<vector<li>> dp;

	explicit PrimeCount(li _n): n(_n), K(max(10., pow(n / max(1., log(n)), 2. / 3))) {
		tie(erat, primes) = sieve(K);
		dp.resize(n / (K - 1) + 1);
		int sz = 0;
		for (int i = 1, j = (int)primes.size() - 1; n / i >= K; ++i) {
			while (j > 0 && sqr(primes[j - 1]) > n / i) {
				--j;
			}
			dp[i].assign(j + 2, def);
			sz += dp[i].size();
		}
		sorted_numbers.resize(K - 1);
		iota(all(sorted_numbers), 1);
		sort(all(sorted_numbers), [&](int i, int j) {
			return erat[i] > erat[j];
		});
	}

	struct Query {
		int id;
		int n;
		int k;
		li ans;
	};

	li calc(int i) {
		vector<Query> qrs;
		fill_queries(i, (int)dp[i].size(), qrs);
		sort(all(qrs), [&](const Query& a, const Query& b) {
			return a.k > b.k;
		});
		Fenwick<li> f(K);
		f.add(1, 1);	// here
		int idx = 0;
		for (int k : sorted_numbers) {
			if (idx >= (int)qrs.size()) {
				break;
			}
			while (idx < (int)qrs.size() && erat[k] < primes[qrs[idx].k]) {
				qrs[idx].ans = f.get(qrs[idx].n);
				++idx;
			}
			f.add(k, 1);	// here
		}
		while (idx < (int)qrs.size()) {
			qrs[idx].ans = f.get(qrs[idx].n);
			++idx;
		}
		sort(all(qrs), [&](const Query& a, const Query& b) {
			return a.id < b.id;
		});
		return calc_queries(i, (int)dp[i].size(), qrs) + (int)dp[i].size() - 1;	// here
	}

private:
	void fill_queries(li i, int j, vector<Query>& qrs) {
		const li curn = n / i;
		if (j == 0) {
			return;
		} else if (curn < K) {
			int sz = qrs.size();
			qrs.push_back({sz, (int)curn, j, 0ll});
			return;
		} else if (j >= (int)dp[i].size()) {
			fill_queries(i, (int)dp[i].size() - 1, qrs);
		} else if (dp[i][j] != def) {
			return;
		} else {
			fill_queries(i * primes[j - 1], j - 1, qrs);
			fill_queries(i, j - 1, qrs);
		}
	}

	li calc_queries(li i, int j, vector<Query>& qrs) {
		const li curn = n / i;
		if (j == 0) {
			return curn;	// here
		} else if (curn < K) {
			li ans = qrs.back().ans;
			qrs.pop_back();
			// cerr << "there are " << ans << " numbers <= " << curn << " with min prime >= " << primes[j] << "\n";
			return ans;
		} else if (j >= (int)dp[i].size()) {
			return calc_queries(i, (int)dp[i].size() - 1, qrs) + ((int)dp[i].size() - 1) - j;
		} else if (dp[i][j] != def) {
			return dp[i][j];
		} else {
			li ans = 0;
			ans += calc_queries(i, j - 1, qrs);
			ans -= calc_queries(i * primes[j - 1], j - 1, qrs);
			// cerr << "there are " << ans << " numbers <= " << curn << " with min prime >= " << primes[j] << "\n";
			return dp[i][j] = ans;
		}
	}
};
