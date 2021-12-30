#pragma once

#include <array>
#include <string>
#include <utility>
#include <vector>

using std::array, std::vector, std::pair, std::string;

template <int N>
struct Hasher {
	using Hash = array<int, N>;
	array<int, N> mods;
	vector<Hash> pwrs;
	vector<Hash> h;

	Hasher(const array<pair<int, int>, N>& bases, const string& s) {
		const int n = s.size();
		for (int i = 0; i < N; ++i) {
			mods[i] = bases[i].first;
		}
		pwrs.resize(n + 1);
		h.assign(n + 1, {});
		pwrs[0].fill(1);
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < N; ++j) {
				pwrs[i + 1][j] = (1ll * pwrs[i][j] * bases[j].second) % mods[j];
				h[i + 1][j] = (1ll * h[i][j] * bases[j].second + s[i]) % mods[j];
			}
		}
	}

	Hash get(int l, int r) const {
		Hash res = h[r];
		for (int i = 0; i < N; ++i) {
			res[i] = (res[i] - 1ll * h[l][i] * pwrs[r - l][i] % mods[i] + mods[i]) % mods[i];
		}
		return res;
	}
};