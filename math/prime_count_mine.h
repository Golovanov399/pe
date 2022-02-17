#pragma once

#include "sieve.h"
#include "../ds/fenwick.h"
#include "../base/defines.h"

#include <cmath>
#include <functional>

using std::function;

[[deprecated("better use prime_count(n) from prime_count_cheat.h")]] long long count_primes(long long n) {
	if (n <= 1000) {
		return sieve(n).second.size();
	}
	long long y = pow(n, 0.36);
	const int mx = n / y;
	vector<int> erat, primes;
	tie(erat, primes) = sieve(mx);
	int pi_y = upper_bound(all(primes), y) - primes.begin();
	long long ans = pi_y - 1;
	{
		int l = pi_y;
		int r = (int)primes.size() - 1;
		while (true) {
			while (r >= l && primes[l] > n / primes[r]) {
				--r;
			}
			if (r < l) {
				break;
			}
			ans -= r - l + 1;
			++l;
		}
	}

	struct Event {
		long long m;
		int a;
		int sign;
	};

	vector<Event> events;
	function<void(long long, int, int)> store_phi = [&](long long m, int a, int sign) {
		if (a == 0) {
			ans += sign * m;
			return;
		}
		if (m <= mx) {
			events.push_back({m, a, sign});
			return;
		}
		store_phi(m, a - 1, sign);
		store_phi(m / primes[a - 1], a - 1, -sign);
	};
	store_phi(n, pi_y, 1);
	sort(all(events), [&](const Event& a, const Event& b) {
		return a.a > b.a;
	});
	vector<int> pidx(mx + 1, -1);
	for (int i = 0; i < (int)primes.size(); ++i) {
		pidx[primes[i]] = i;
	}
	vector<vector<int>> by_p(primes.size());
	for (int i = 2; i <= mx; ++i) {
		by_p[pidx[erat[i]]].push_back(i);
	}
	Fenwick<int> f(mx + 1);
	f.add(1, 1);
	int last = (int)primes.size();
	for (const auto& e : events) {
		while (last > e.a) {
			--last;
			for (int x : by_p[last]) {
				f.add(x, 1);
			}
		}
		ans += e.sign * f.get(e.m);
	}
	return ans;
}
