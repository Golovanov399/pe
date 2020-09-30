#include <bits/stdc++.h>
#include "math/modular.h"
#include "math/ntt.h"
#include "math/prime.h"

using namespace std;

constexpr int mod = 998'244'353;
using Mint = Modular<mod>;

int main() {
	NTT<mod, (1 << 21)> ntt;
	int n = nxt();
	vector<Mint> a(n);
	cin >> a;
	int zc = 0;
	while (zc < n && !a[zc]) {
		++zc;
	}
	if (zc == n) {
		cout << a << "\n";
		return 0;
	}
	if (zc % 2) {
		cout << "-1\n";
		return 0;
	}
	a.erase(a.begin(), a.begin() + zc);
	a.resize(n - zc / 2);
	Mint k = a[0];
	k = k.inv();
	for (auto& x : a) {
		x *= k;
	}
	k = k.inv();
	if (!is_square_residue((int)k, mod)) {
		cout << "-1\n";
		return 0;
	}
	k = sqrt_mod((int)k, mod);
	a = ntt.log(a, a.size());
	for (auto& x : a) {
		x /= 2;
	}
	for (int i = (int)a.size(); i < n; ++i) {
		cout << "0 ";
	}
	a = ntt.exp(a, a.size());
	for (auto& x : a) {
		x *= k;
	}
	cout << a << "\n";

	return 0;
}
