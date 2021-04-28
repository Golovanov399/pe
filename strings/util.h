#pragma once

#include "../base/base.h"

vector<int> z_function(const string& s) {
	const int n = s.size();
	vector<int> z(n);
	int opt = 0;
	for (int i = 1; i < n; ++i) {
		if (i < opt + z[opt]) {
			z[i] = min(z[i - opt], opt + z[opt] - i);
		}
		while (i + z[i] < n && s[i + z[i]] == s[z[i]]) {
			++z[i];
		}
		if (opt + z[opt] < i + z[i]) {
			opt = i;
		}
	}
	z[0] = n;
	return z;
}

vector<int> prefix_function(const string& s) {
	const int n = s.size();
	vector<int> p(n + 1);
	for (int i = 2; i <= n; ++i) {
		p[i] = p[i - 1] + 1;
		while (p[i] > 1 && s[i - 1] != s[p[i] - 1]) {
			p[i] = p[p[i] - 1] + 1;
		}
		if (p[i] == 1 && s[i - 1] != s[0]) {
			p[i] = 0;
		}
	}
	return p;
}

vector<int> manacher(const string& s) {
	const int n = s.size();
	if (!n) {
		return {};
	}
	vector<int> res(2 * n - 1);
	int opt = 0;
	for (int i = 0; i < 2 * n - 1; ++i) {
		if (i % 2 == 0) {
			res[i] = 1;
		}
		if (opt + res[opt] > i + res[i]) {
			res[i] = min(res[2 * opt - i], res[i] + (opt + res[opt] - res[i] - i) / 2 * 2);
		}
		int l = (i - res[i] - 1) / 2;
		int r = (i + res[i] + 1) / 2;
		while (l >= 0 && r < n && s[l] == s[r]) {
			res[i] += 2;
			--l, ++r;
		}
		if (i + res[i] > opt + res[opt]) {
			opt = i;
		}
	}
	return res;
}