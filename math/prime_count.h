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
	vector<li> prec;

	explicit PrimeCount(li _n): n(_n), K(max({20., pow(n / max(1., log(n)), 2. / 3), sqrt(n) + 100})) {
		tie(erat, primes) = sieve(K);
		dp.resize(n / (K - 1) + 1);
		for (int i = 1, j = (int)primes.size() - 1; n / i >= K; ++i) {
			while (j > 0 && sqr(primes[j - 1]) > n / i) {
				--j;
			}
			dp[i].assign(j + 2, def);
		}
		while (!dp.empty() && dp.back().empty()) {
			dp.pop_back();
		}
		sorted_numbers.resize(K - 1);
		iota(all(sorted_numbers), 1);
		sort(all(sorted_numbers), [&](int i, int j) {
			return erat[i] > erat[j];
		});

		prec.resize(K);
		for (int p : primes) {
			prec[p] += 1;	// here
		}
		for (int i = 1; i < (int)prec.size(); ++i) {
			prec[i] += prec[i - 1];
		}
	}

	struct Query {
		int id;
		int n;
		int k;
		li ans;
	};

	void fill_dp() {
		if (dp.empty()) {
			return;
		}
		Fenwick<li> f(K);
		for (int i = 1; i < K; ++i) {
			f.add(i, 1);	// here
		}
		for (auto& v : dp) {
			fill(all(v), 0);
		}
		int idx = (int)sorted_numbers.size() - 2;
		for (int j = 0; j < (int)dp[1].size(); ++j) {
			for (int i = 1; i < (int)dp.size() && j < (int)dp[i].size(); ++i) {
				if (j > 0 && n / i >= K && i * primes[j - 1] >= (int)dp.size()) {
					dp[i][j] -= f.get(n / i / primes[j - 1]);
				}
			}
			while (idx >= 0 && erat[sorted_numbers[idx]] < primes[j]) {
				f.add(sorted_numbers[idx], -1);	// here
				--idx;
			}
			for (int i = 1; i < (int)dp.size() && j < (int)dp[i].size(); ++i) {
				if (j == 0) {
					dp[i][j] += n / i;	// here
				} else if (n / i < K) {
					dp[i][j] += f.get(n / i);
				} else {
					dp[i][j] += dp[i][j - 1];
					if (li ni = i * primes[j - 1]; ni < (int)dp.size()) {
						int mn = min(j - 1, (int)dp[ni].size() - 1);
						dp[i][j] -= dp[ni][mn] + mn - (j - 1);
					}
				}
			}
		}
	}

	li calc(int i) {
		if (n / i < (int)prec.size()) {
			return prec[n / i];
		}
		vector<Query> qrs;
		fill_queries(i, (int)dp[i].size(), qrs);
		if (!qrs.empty()) {
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
		}
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
			return ans;
		} else if (j >= (int)dp[i].size()) {
			return calc_queries(i, (int)dp[i].size() - 1, qrs) + ((int)dp[i].size() - 1) - j;
		} else if (dp[i][j] != def) {
			return dp[i][j];
		} else {
			li ans = 0;
			ans += calc_queries(i, j - 1, qrs);
			ans -= calc_queries(i * primes[j - 1], j - 1, qrs);
			return dp[i][j] = ans;
		}
	}
};
