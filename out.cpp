#include <bits/stdc++.h>

using namespace std;



#define all(x) (x).begin(), (x).end()
#define make_unique(x) sort((x).begin(), (x).end()); (x).erase(unique((x).begin(), (x).end()), (x).end())


using li = long long;
using LI = __int128_t;
using ld = long double;
using ull = unsigned long long;

LI gcd(LI x, LI y) {
	while (y) {
		x %= y;
		swap(x, y);
	}
	return x;
}

LI lcm(LI x, LI y) {
	return x / gcd(x, y) * y;
}

ostream& operator <<(ostream& ostr, LI x) {
	static constexpr li BIG = 1e18;
	if (x < 0) {
		ostr << "-";
		x = -x;
	}
	if (x < BIG) {
		return ostr << (li)x;
	} else {
		stringstream ss;
		ss << setfill('0') << setw(18) << (li)(x % BIG);
		return ostr << (li)(x / BIG) << ss.str();
	}
}

template <typename T>
ostream& operator <<(ostream& ostr, const vector<T>& vec) {
	auto [pre, sep, post] = (&ostr == &cout) ? make_tuple("", " ", "") : make_tuple("[", ", ", "]");
	ostr << pre;
	bool fp = true;
	for (const auto& x : vec) {
		if (fp) {
			fp = false;
		} else {
			ostr << sep;
		}
		ostr << x;
	}
	return ostr << post;
}

template <typename T>
ostream& operator <<(ostream& ostr, const set<T>& st) {
	auto [pre, sep, post] = (&ostr == &cout) ? make_tuple("", " ", "") : make_tuple("{", ", ", "}");
	ostr << pre;
	bool fp = true;
	for (const auto& x : st) {
		if (fp) {
			fp = false;
		} else {
			ostr << sep;
		}
		ostr << x;
	}
	return ostr << post;
}

template <typename T>
ostream& operator <<(ostream& ostr, const multiset<T>& st) {
	auto [pre, sep, post] = (&ostr == &cout) ? make_tuple("", " ", "") : make_tuple("{", ", ", "}");
	ostr << pre;
	bool fp = true;
	for (const auto& x : st) {
		if (fp) {
			fp = false;
		} else {
			ostr << sep;
		}
		ostr << x;
	}
	return ostr << post;
}

template <typename T, typename U>
ostream& operator <<(ostream& ostr, const map<T, U>& mp) {
	ostr << "{";
	bool fp = true;
	for (const auto& [k, v] : mp) {
		if (fp) {
			fp = false;
		} else {
			ostr << ", ";
		}
		ostr << k << ": " << v;
	}
	return ostr << "}";
}

template <typename T, typename U>
ostream& operator <<(ostream& ostr, const pair<T, U>& p) {
	return ostr << "(" << p.first << ", " << p.second << ")";
}

#define imie(x) #x << ": " << x

template <typename T>
istream& operator >>(istream& istr, vector<T>& vec) {
	for (auto& x : vec) {
		istr >> x;
	}
	return istr;
}

template <typename T = int>
T nxt(istream& istr = cin) {
	T x;
	istr >> x;
	return x;
}

template <typename T>
bool is_zero(const T& x) {
	if (is_integral_v<T>) {
		return x == 0;
	} else if (is_floating_point_v<T>) {
		return abs(x) < 1e-8;
	} else {
		assert(false);
	}
}

template <typename T>
T pw(T a, li b) {
	T res = 1;
	while (b) {
		if (b & 1ll) {
			res *= a;
		}
		b >>= 1;
		a *= a;
	}
	return res;
}
template <typename T>
struct TypeModular {
	using Type = typename decay<decltype(T::value)>::type;
	Type val;

	static constexpr Type mod() {
		return T::value;
	}

	TypeModular(long long _val = 0) {
		if (_val < 0 || _val >= mod()) {
			val = _val % mod();
			if (val < 0) {
				val += mod();
			}
		} else {
			val = _val;
		}
	}

	friend TypeModular operator -(const TypeModular& a) {
		return {-a.val};
	}

	friend TypeModular operator +(const TypeModular& a, const TypeModular& b) {
		Type res = a.val + b.val;
		if (res >= mod()) {
			res -= mod();
		}
		return {res};
	}

	friend TypeModular operator -(const TypeModular& a, const TypeModular& b) {
		Type res = a.val - b.val;
		if (res < 0) {
			res += mod();
		}
		return {res};
	}

	friend TypeModular operator *(const TypeModular& a, const TypeModular& b) {
		return {1ll * a.val * b.val};
	}

	friend TypeModular operator /(const TypeModular& a, const TypeModular& b) {
		return a * b.inv();
	}

	TypeModular& operator +=(const TypeModular& b) {
		val += b.val;
		if (val >= mod()) {
			val -= mod();
		}
		return *this;
	}

	TypeModular& operator -=(const TypeModular& b) {
		val -= b.val;
		if (val < 0) {
			val += mod();
		}
		return *this;
	}

	TypeModular& operator *=(const TypeModular& b) {
		val = 1ll * val * b.val % mod();
		return *this;
	}

	TypeModular& operator /=(const TypeModular& b) {
		*this *= b.inv();
		return *this;
	}

	TypeModular& operator ++() {
		val += 1;
		if (val == mod()) {
			val = 0;
		}
		return *this;
	}

	TypeModular operator ++(int) {
		TypeModular tmp(*this);
		operator ++();
		return tmp;
	}

	friend bool operator ==(const TypeModular& a, const TypeModular& b) {
		return a.val == b.val;
	}

	friend bool operator !=(const TypeModular& a, const TypeModular& b) {
		return a.val != b.val;
	}

	TypeModular pow(long long b) const {
		TypeModular res{1}, a{*this};
		while (b) {
			if (b & 1ll) {
				res *= a;
			}
			b >>= 1;
			a *= a;
		}
		return res;
	}

	TypeModular inv() const {
		assert(val);
		return _inv(val, mod());
	}

	int _inv(int a, int b) const {
		return a == 1 ? a : b - 1ll * _inv(b % a, a) * b / a % b;
	}

	explicit operator Type() const {
		return val;
	}

	explicit operator bool() const {
		return val;
	}
};

template <typename T = int>
struct integral_variable {
	static T value;
};

template <typename T> T integral_variable<T>::value;

void set_prime(int p) {
	integral_variable<int>::value = p;
}

template <typename T>
ostream& operator <<(ostream& ostr, const TypeModular<T>& x) {
	return ostr << x.val;
}

template <typename T>
istream& operator >>(istream& istr, TypeModular<T>& x) {
	x = nxt(istr);
	return istr;
}

template <int mod>
using Modular = TypeModular<integral_constant<int, mod>>;


template <typename outer_type, typename inner_type, int N>
class IFFT {
public:
	IFFT(): initialized_(false) {}

	~IFFT() {}

	virtual vector<outer_type> multiply(vector<outer_type> a, vector<outer_type> b) {
		if (!initialized_) {
			initialize();
		}
		int n = 1;
		while (n < (int)a.size() || n < (int)b.size()) {
			n *= 2;
		}
		vector<inner_type> ar(n + n), br(n + n);
		if constexpr (is_convertible_v<outer_type, inner_type>) {
			copy(all(a), ar.begin());
			copy(all(b), br.begin());
		} else {
			throw runtime_error("please, implement your own child multiply function");
		}
		fft(ar);
		fft(br);
		for (int i = 0; i < (int)ar.size(); ++i) {
			ar[i] *= br[i];
		}
		ifft(ar);
		vector<outer_type> res((int)a.size() + (int)b.size() - 1);
		assert(res.size() <= ar.size());
		if constexpr (is_convertible_v<inner_type, outer_type>) {
			copy_n(ar.begin(), res.size(), res.begin());
		} else {
			throw runtime_error("please, implement your own child multiply function");
		}
		return res;
	}

	vector<outer_type> inverse(const vector<outer_type>& a, int prec) {
		assert(!a.empty());
		assert(a[0] != 0);
		vector<outer_type> b = {1 / a[0]};
		for (int len = 1; len < prec; len *= 2) {
			auto tmp = multiply(b, b);
			tmp = multiply(tmp, vector<outer_type>{a.begin(), a.begin() + min(2 * len, (int)a.size())});
			tmp.resize(2 * len);
			for (int i = 0; i < len; ++i) {
				tmp[i] = 2 * b[i] - tmp[i];
				tmp[len + i] = -tmp[len + i];
			}
			b.swap(tmp);
		}
		b.resize(prec);
		return b;
	}

	vector<outer_type> derivative(vector<outer_type> a) {
		if (a.empty()) {
			return a;
		}
		a.erase(a.begin());
		for (int i = 0; i < (int)a.size(); ++i) {
			a[i] *= i + 1;
		}
		return a;
	}

	vector<outer_type> primitive(vector<outer_type> a) {
		a.insert(a.begin(), 0);
		for (int i = 1; i < (int)a.size(); ++i) {
			a[i] /= i;
		}
		return a;
	}

	vector<outer_type> log(const vector<outer_type>& a, int prec) {
		assert(!a.empty());
		assert(a[0] == 1);
		auto res = primitive(multiply(derivative(a), inverse(a, prec)));
		res.resize(prec);
		return res;
	}

	vector<outer_type> exp(const vector<outer_type>& a, int prec) {
		assert(!a.empty());
		assert(a[0] == 0);
		vector<outer_type> b = {1};
		for (int len = 1; len < prec; len *= 2) {
			auto tmp = vector<outer_type>{a.begin(), a.begin() + min(2 * len, (int)a.size())};
			tmp.resize(2 * len);
			tmp[0] += 1;
			auto l = log(b, 2 * len);
			for (int i = 0; i < 2 * len; ++i) {
				tmp[i] -= l[i];
			}
			b = multiply(tmp, b);
			b.resize(2 * len);
		}
		b.resize(prec);
		return b;
	}

protected:
	static constexpr int L = 31 - __builtin_clz(N);
	static_assert(!(N & (N - 1)));
	vector<inner_type> angles;
	vector<int> bitrev;
	bool initialized_;

	void initialize() {
		fill_angles();
		fill_bitrev();
		initialized_ = true;
	}

	virtual void fill_angles() = 0;

	void fill_bitrev() {
		bitrev.assign(N, 0);
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < L; ++j) {
				bitrev[i] = (bitrev[i] << 1) | !!(i & (1 << j));
			}
		}
	}

	void butterfly(vector<inner_type>& a) const {
		const int n = a.size();
		assert(!(n & (n - 1)));
		const int l = __builtin_ctz(n);

		for (int i = 0; i < n; ++i) {
			int j = revbit(i, l);
			if (j > i) {
				swap(a[i], a[j]);
			}
		}
	}

	int revbit(int num, int len) const {
		return bitrev[num] >> (L - len);
	}

	virtual void fft(vector<inner_type>& a) const {
		const int n = a.size();
		assert(!(n & (n - 1)));
		butterfly(a);

		for (int len = 1; len < n; len *= 2) {
			for (int start = 0; start < n; start += 2 * len) {
				for (int i = 0; i < len; ++i) {
					const auto w = angles[N / 2 / len * i];
					const auto x = a[start + i], y = a[start + len + i] * w;
					a[start + i] = x + y;
					a[start + len + i] = x - y;
				}
			}
		}
	}

	void ifft(vector<inner_type>& a) const {
		fft(a);
		for (auto& x : a) {
			x /= a.size();
		}
		reverse(1 + all(a));
	}
};
template <int mod, int N = (1 << __builtin_ctz(mod - 1))>
class NTT : public IFFT<Modular<mod>, Modular<mod>, N> {
	using Mint = Modular<mod>;
protected:
	void fill_angles() {
		vector<int> primes;
		for (int x = mod - 1, p = 2; x > 1; ++p) {
			if (p * p > x) {
				primes.push_back(x);
				break;
			}
			if (x % p == 0) {
				primes.push_back(p);
				while (x % p == 0) {
					x /= p;
				}
			}
		}
		auto isPrimitiveRoot = [&](Mint g) {
			for (int p : primes) {
				if (g.pow(mod / p) == 1) {
					return false;
				}
			}
			return true;
		};
		int g = 2;
		while (!isPrimitiveRoot(g)) {
			++g;
		}
		g = Mint(g).pow(mod / N).val;

		this->angles.assign(N, 1);
		for (int i = 1; i < N; ++i) {
			this->angles[i] = this->angles[i - 1] * g;
		}
		assert(this->angles.back() * g == 1);
	}

	void ntt(vector<Mint>& a) {
		fft(a);
	}
};



bool is_prime(int n) {
	if (n == 1) {
		return false;
	}
	for (int i = 2; i * i <= n; ++i) {
		if (n % i == 0) {
			return false;
		}
	}
	return true;
}

bool is_square_residue(int a, int p) {
	assert(is_prime(p));
	if (!a) {
		return true;
	}
	if (p == 2) {
		return true;
	}
	return pw(TypeModular<integral_variable<int>>(a), (p - 1) / 2) == 1;
}

int sqrt_mod(int a, int p) {
	if (!is_square_residue(a, p)) {
		return -1;
	}
	if (a == 0) {
		return 0;
	}
	if (p == 2) {
		return a;
	}
	using Mint = TypeModular<integral_variable<int>>;
	if (p % 4 == 3) {
		return (int)pw(Mint(a), (p + 1) / 4);
	}

	// tonelli-shanks starts here
	Mint z = 1;
	while (pw(z, (p - 1) / 2) == 1) {
		++z;
	}
	int q = p - 1;
	int s = __builtin_ctz(q);
	q >>= s;
	int m = s;
	Mint c = z.pow(q);
	Mint t = Mint(a).pow(q);
	Mint r = Mint(a).pow((q + 1) / 2);
	while (true) {
		if (t == 0) {
			return 0;
		}
		if (t == 1) {
			return (int)r;
		}
		auto tmp = t;
		int i = 0;
		while (tmp != 1) {
			i += 1;
			tmp *= tmp;
		}
		Mint b = c;
		for (int j = 0; j < m - i - 1; ++j) {
			b *= b;
		}
		r *= b;
		c = b * b;
		t *= c;
		m = i;
	}
}

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
	set_prime(mod);
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

	return 0;
}

