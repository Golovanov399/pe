#pragma once

#include "prime_count_cheat.h"
#include "prime_count_mine.h"

#include "sieve.h"
#include "modular.h"
#include "../base/util.h"
#include "../ds/fenwick.h"

// Let h be a completely multiplicative function, hp(n) = sum_{i=1}^n h(i).
// Then this class allows one to calculate sum_{i=1}^{[n/k]} [i is prime] * h(i) for every k.
// Provided function _h only needs to equal h on primes
template <typename T, typename FuncH, typename FuncHp>
class PrimePrefix {
public:
	const li n;
	const li K;
	const FuncH h;
	const FuncHp hp;

	const T def = 2281488;

	vector<int> erat, primes;
	vector<int> sorted_numbers;
	vector<vector<T>> dp;
	vector<T> prec;
	vector<T> h_small;

	explicit PrimePrefix(li _n, FuncH&& _h = FuncH(), FuncHp&& _hp = FuncHp()): n(_n), K(max({20., pow(n / max(1., log(n)), 2. / 3), sqrt(n) + 100})), h(_h), hp(_hp) {
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

		prec.resize(K + 1);
		for (int p : primes) {
			prec[p] += h(p);
		}
		for (int i = 1; i < (int)prec.size(); ++i) {
			prec[i] += prec[i - 1];
		}
		h_small.resize(K + 1, 1);
		for (int i = 2; i <= K; ++i) {
			h_small[i] = h_small[i / erat[i]] * h(erat[i]);
		}
	}

	struct Query {
		int id;
		int n;
		int k;
		T ans;
	};

	void fill_dp() {
		if (dp.empty()) {
			return;
		}
		Fenwick<T> f(K);
		for (int i = 1; i < K; ++i) {
			f.add(i, h_small[i]);
		}
		for (auto& v : dp) {
			fill(all(v), 0);
		}
		int idx = (int)sorted_numbers.size() - 2;
		for (int j = 0; j < (int)dp[1].size(); ++j) {
			for (int i = 1; i < (int)dp.size() && j < (int)dp[i].size(); ++i) {
				if (j > 0 && n / i >= K && 1ll * i * primes[j - 1] >= (int)dp.size()) {
					dp[i][j] -= f.get(n / i / primes[j - 1]) * h(primes[j - 1]);
				}
			}
			while (idx >= 0 && erat[sorted_numbers[idx]] < primes[j]) {
				f.add(sorted_numbers[idx], -h_small[sorted_numbers[idx]]);
				--idx;
			}
			for (int i = 1; i < (int)dp.size() && j < (int)dp[i].size(); ++i) {
				if (j == 0) {
					dp[i][j] += hp(n / i);
				} else if (n / i < K) {
					dp[i][j] += f.get(n / i);
				} else {
					dp[i][j] += dp[i][j - 1];
					if (li ni = 1ll * i * primes[j - 1]; ni < (int)dp.size()) {
						int mn = min(j - 1, (int)dp[ni].size() - 1);
						dp[i][j] -= (dp[ni][mn] + prec[primes[mn] - 1] - prec[primes[j - 1] - 1]) * h(primes[j - 1]);
					}
				}
			}
		}
	}

	T calc(long long i) {
		if (n / i < (int)prec.size()) {
			return prec[n / i];
		}
		vector<Query> qrs;
		fill_queries(i, (int)dp[i].size(), qrs);
		if (!qrs.empty()) {
			sort(all(qrs), [&](const Query& a, const Query& b) {
				return a.k > b.k;
			});
			Fenwick<T> f(K);
			f.add(1, 1);
			int idx = 0;
			for (int k : sorted_numbers) {
				if (idx >= (int)qrs.size()) {
					break;
				}
				while (idx < (int)qrs.size() && erat[k] < primes[qrs[idx].k]) {
					qrs[idx].ans = f.get(qrs[idx].n);
					++idx;
				}
				f.add(k, h_small[k]);
			}
			while (idx < (int)qrs.size()) {
				qrs[idx].ans = f.get(qrs[idx].n);
				++idx;
			}
			sort(all(qrs), [&](const Query& a, const Query& b) {
				return a.id < b.id;
			});
		}
		return calc_queries(i, (int)dp[i].size(), qrs) + prec[primes[(int)dp[i].size() - 1]] - 1;
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

	T calc_queries(li i, int j, vector<Query>& qrs) {
		const li curn = n / i;
		if (j == 0) {
			return hp(curn);
		} else if (curn < K) {
			T ans = qrs.back().ans;
			qrs.pop_back();
			return ans;
		} else if (j >= (int)dp[i].size()) {
			return calc_queries(i, (int)dp[i].size() - 1, qrs) + prec[primes[(int)dp[i].size() - 1] - 1] - prec[primes[j] - 1];
		} else if (dp[i][j] != def) {
			return dp[i][j];
		} else {
			T ans = 0;
			ans += calc_queries(i, j - 1, qrs);
			ans -= calc_queries(i * primes[j - 1], j - 1, qrs) * h(primes[j - 1]);
			return dp[i][j] = ans;
		}
	}
};

template <typename T, size_t p>
class PowerClass {
public:
	T operator ()(T x) const {
		return pw(x, p);
	}
};

template <typename T, size_t p>
class RisingClass {
public:
	T operator ()(T x) const {
		T res = 1;
		for (size_t i = 0; i < p; ++i) {
			res *= x + i;
			res /= i + 1;
		}
		return res;
	}
};

using PrimeCount = PrimePrefix<li, PowerClass<li, 0>, PowerClass<li, 1>>;

template <typename T>
using PrimeSum = PrimePrefix<T, PowerClass<T, 1>, RisingClass<T, 2>>;
