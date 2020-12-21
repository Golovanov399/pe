#pragma once

#include "../base/base.h"

pair<vector<int>, vector<int>> sieve(int n) {
	vector<int> erat(n + 1);
	vector<int> primes;
	erat[1] = 1;
	for (int i = 2; i <= n; ++i) {
		if (erat[i] == 0) {
			erat[i] = i;
			primes.push_back(i);
		}
		for (int p : primes) {
			if (p > erat[i] || i * p > n) {
				break;
			}
			erat[i * p] = p;
		}
	}
	return {erat, primes};
}

vector<int> calc_phi(const vector<int>& erat) {
	const int n = erat.size();
	vector<int> phi(n);
	phi[1] = 1;
	for (int i = 2; i < n; ++i) {
		phi[i] = phi[i / erat[i]] * erat[i];
		if (erat[i] == erat[i / erat[i]]) {
			phi[i] -= phi[i / erat[i]];
		}
	}
	return phi;
}

vector<int> calc_mu(const vector<int>& erat) {
	const int n = erat.size();
	vector<int> mu(n);
	mu[1] = 1;
	for (int i = 2; i < n; ++i) {
		mu[i] = (erat[i] == erat[i / erat[i]]) ? 0 : -mu[i / erat[i]];
	}
	return mu;
}
