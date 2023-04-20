#pragma once

#include <algorithm>
#include <cctype>
#include <compare>
#include <cstdint>
#include <string>
#include <vector>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "../base/sign.h"

using std::vector, std::strong_ordering;

struct BigInteger {
	using u32 = uint32_t;
	using u64 = uint64_t;
	using i64 =  int64_t;
	static constexpr u32 BASE = 1'000'000'000;
	static constexpr int BASE_LEN = [](u32 base) { int res = 0; while (base > 1) { base /= 10; ++res; } return res; }(BASE);

	vector<u32> digits;
	bool neg;

	BigInteger(long long x = 0): neg(false) {
		if (x < 0) {
			neg = true;
			x = -x;
		}
		while (x) {
			digits.push_back(x % BASE);
			x /= BASE;
		}
	}

	BigInteger(const std::string& s) {
		neg = false;
		digits = {};
		int i = (int)s.length() - 1;
		while (i >= 0 && std::isdigit(s[i])) {
			int j = i;
			while (j >= 0 && j > i - BASE_LEN && std::isdigit(s[j])) {
				--j;
			}
			digits.push_back(std::stoi(s.substr(j + 1, i - j)));
			i = j;
		}
		if (i > 0 || (i == 0 && s[i] != '-' && s[i] != '+')) {
			throw std::runtime_error("invalid biginteger");
		} else if (i == 0 && s[i] == '-') {
			neg = true;
		}
		shrink();
	}

	BigInteger from_int128(__int128_t x) {
		BigInteger res;
		if (x < 0) {
			res.neg = true;
			x = -x;
		}
		while (x) {
			res.digits.push_back(x % BASE);
			x /= BASE;
		}
		return res;
	}

	static BigInteger from_digits(const vector<u32>& dgs, bool neg = false) {
		BigInteger res;
		res.digits = dgs;
		res.neg = neg;
		return res;
	}

	void shrink() {
		while (!digits.empty() && !digits.back()) {
			digits.pop_back();
		}
		if (digits.empty()) {
			neg = false;
		}
	}

	int _cmp(const BigInteger& ot) const {
		if (digits.size() != ot.digits.size()) {
			return sign((int)digits.size() - (int)ot.digits.size());
		}
		for (int i = (int)digits.size() - 1; i >= 0; --i) {
			if (digits[i] != ot.digits[i]) {
				return sign((int)digits[i] - (int)ot.digits[i]);
			}
		}
		return 0;
	}

	void _add(const vector<u32>& dgs) {
		u32 carry = 0;
		if (dgs.size() > digits.size()) {
			digits.resize(dgs.size());
		}
		for (int i = 0; i < (int)digits.size() || i < (int)dgs.size(); ++i) {
			if (i < (int)digits.size()) {
				carry += digits[i];
			}
			if (i < (int)dgs.size()) {
				carry += dgs[i];
			}
			digits[i] = carry % BASE;
			carry /= BASE;
		}
		if (carry) {
			digits.push_back(carry);
		}
	}

	void _sub(const vector<u32>& dgs) {
		u32 carry = 0;
		for (int i = 0; i < (int)digits.size(); ++i) {
			if (i < (int)dgs.size()) {
				carry += dgs[i];
			}
			if (digits[i] >= carry) {
				digits[i] -= carry;
				carry = 0;
			} else {
				digits[i] += BASE;
				digits[i] -= carry;
				carry = 1;
			}
		}
		shrink();
	}

	static vector<u32> _mul(const vector<u32>& a, const vector<u32>& b) {
		if (a.empty() || b.empty()) {
			return {};
		}
		vector<u32> res(a.size() + b.size() - 1);
		u64 carry = 0;
		for (int i = 0; i < (int)a.size() + (int)b.size() - 1; ++i) {
			res[i] = carry % BASE;
			carry /= BASE;
			int from = std::max(0, i - (int)b.size() + 1);
			int to = std::min((int)a.size() - 1, i);
			for (int j = from; j <= to; ++j) {
				u64 tmp = (u64)a[j] * b[i - j];
				carry += tmp / BASE;
				res[i] += tmp % BASE;
				if (res[i] >= BASE) {
					res[i] -= BASE;
					carry += 1;
				}
			}
		}
		while (carry > 0) {
			res.push_back(carry % BASE);
			carry /= BASE;
		}
		return res;
	}

	static int _len(const vector<u32>& dgs) {
		if (dgs.empty()) {
			return 0;
		} else {
			return ((int)dgs.size() - 1) * BASE_LEN + std::to_string(dgs.back()).length();
		}
	}

	static vector<u32> _div(vector<u32> a, const vector<u32>& b) {
		vector<u32> res;
		u64 cur = 0;
		for (int i = (int)a.size() - 1; i >= (int)b.size() - 1; --i) {
			cur *= BASE;
			cur += a[i];
			u32 le = cur / (b.back() + 1);
			u32 ri = cur / b.back() + 1;
			while (ri > le + 1) {
				u32 k = le + (ri - le) / 2;

				u64 can = cur;
				bool ok = true;
				for (int j = 0; j < (int)b.size(); ++j) {
					if (j) {
						can = can * BASE + a[i - j];
					}
					if (can < (u64)b[(int)b.size() - 1 - j] * k) {
						ok = false;
						break;
					}
					can -= (u64)b[(int)b.size() - 1 - j] * k;
					if (can >= k) {
						break;
					}
				}
				(ok ? le : ri) = k;
			}
			u32 k = le;
			res.push_back(k);
			u64 carry = 0;
			for (int j = (int)b.size() - 1; j > 0; --j) {
				carry += (u64)b[(int)b.size() - 1 - j] * k;
				if (carry % BASE <= a[i - j]) {
					a[i - j] -= carry % BASE;
				} else {
					a[i - j] += BASE;
					a[i - j] -= carry % BASE;
					carry += BASE;
				}
				carry /= BASE;
			}
			carry += (u64)b.back() * k;
			cur -= carry;
		}
		reverse(res.begin(), res.end());
		return res;
	}

	BigInteger& operator +=(const BigInteger& ot) {
		if (neg == ot.neg) {
			_add(ot.digits);
		} else if (_cmp(ot) < 0) {
			neg = ot.neg;
			auto d = digits;
			digits = ot.digits;
			_sub(d);
		} else {
			_sub(ot.digits);
		}
		return *this;
	}

	BigInteger& operator -=(const BigInteger& ot) {
		if (neg == !ot.neg) {
			_add(ot.digits);
		} else if (_cmp(ot) < 0) {
			neg = !ot.neg;
			auto d = digits;
			digits = ot.digits;
			_sub(d);
		} else {
			_sub(ot.digits);
		}
		return *this;
	}

	BigInteger operator +(const BigInteger& ot) const {
		BigInteger res = *this;
		res += ot;
		return res;
	}

	BigInteger operator -(const BigInteger& ot) const {
		BigInteger res = *this;
		res -= ot;
		return res;
	}

	BigInteger operator -() const {
		auto res = *this;
		if (!digits.empty()) {
			res.neg ^= 1;
		}
		return res;
	}

	BigInteger& operator *=(const BigInteger& ot) {
		if (digits.empty() || ot.digits.empty()) {
			digits = {};
			neg = false;
			return *this;
		}
		digits = _mul(digits, ot.digits);
		neg ^= ot.neg;
		return *this;
	}

	BigInteger operator *(const BigInteger& ot) const {
		if (digits.empty() || ot.digits.empty()) {
			return from_digits({});
		}
		return from_digits(_mul(digits, ot.digits), neg ^ ot.neg);
	}

	BigInteger& operator /=(i64 x) {	// negative numbers are divided as in C++
		if (x == 0) {
			throw std::domain_error("division by zero");
		}
		if (x < 0) {
			neg ^= 1;
			x = -x;
		}
		u64 carry = 0;
		for (int i = (int)digits.size() - 1; i >= 0; --i) {
			carry *= BASE;
			carry += digits[i];
			digits[i] = carry / x;
			carry %= x;
		}
		shrink();
		return *this;
	}

	BigInteger operator /(i64 x) const {
		auto res = *this;
		res /= x;
		return res;
	}

	i64 operator %(i64 x) const {
		if (x == 0) {
			throw std::domain_error("division by zero");
		}
		bool flip = neg;
		if (x < 0) {
			flip ^= 1;
			x = -x;
		}
		u64 carry = 0;
		for (int i = (int)digits.size() - 1; i >= 0; --i) {
			carry *= BASE;
			carry += digits[i];
			carry %= x;
		}
		return (i64)carry * (flip ? -1 : 1);
	}

	BigInteger& operator %=(i64 x) {
		*this = *this % x;
		return *this;
	}

	BigInteger& operator /=(const BigInteger& ot) {
		if (ot.digits.empty()) {
			throw std::domain_error("division by zero");
		}
		if (digits.empty()) {
			return *this;
		}
		neg ^= ot.neg;
		digits = _div(digits, ot.digits);
		shrink();
		return *this;
	}

	BigInteger operator /(const BigInteger& ot) const {
		auto res = *this;
		res /= ot;
		return res;
	}

	strong_ordering operator <=>(const BigInteger& ot) const {
		if (neg != ot.neg) {
			return neg ? strong_ordering::less : strong_ordering::greater;
		} else {
			int s = _cmp(ot);
			if (neg) {
				s = -s;
			}
			return s < 0 ? strong_ordering::less : s > 0 ? strong_ordering::greater : strong_ordering::equal;
		}
	}

	bool operator ==(const BigInteger& ot) const {
		return neg == ot.neg && digits == ot.digits;
	}

	BigInteger pow(u32 x) const {
		if (!x) {
			return 1;
		}
		BigInteger res = 1;
		for (int i = 31 - __builtin_clz(x); i >= 0; --i) {
			res *= res;
			if ((x >> i) & 1) {
				res *= *this;
			}
		}
		return res;
	}

	std::string to_string() const {
		std::string s;
		std::ostringstream ss(s);
		ss << *this;
		return s;
	}

	void drop_digits(int cnt) {
		if (!cnt) {
			return;
		}
		int blocks = cnt / BASE_LEN;
		if (blocks >= (int)digits.size()) {
			digits = {};
			shrink();
			return;
		}
		digits.erase(digits.begin(), digits.begin() + blocks);
		int rem = cnt - blocks * BASE_LEN;
		if (rem) {
			const u32 tp = [](int deg) { u32 res = 1; while (deg--) { res *= 10; } return res; }(rem);
			for (int i = 0; i + 1 < (int)digits.size(); ++i) {
				digits[i] = digits[i] / tp + (digits[i + 1] % tp) * (BASE / tp);
			}
			digits.back() /= tp;
			shrink();
		}
	}

	void add_zeroes(int cnt) {
		if (digits.empty()) {
			return;
		}
		int blocks = (cnt + BASE_LEN - 1) / BASE_LEN;
		digits.resize(digits.size() + blocks);
		std::copy(digits.begin(), digits.begin() + (int)digits.size() - blocks, digits.begin() + blocks);
		std::fill(digits.begin(), digits.begin() + blocks, 0);
		drop_digits(blocks * BASE_LEN - cnt);
	}

	void leave_digits(int cnt) {
		int blocks = (cnt + BASE_LEN - 1) / BASE_LEN;
		if (blocks > (int)digits.size()) {
			return;
		}
		digits.resize(blocks);
		if (cnt != blocks * BASE_LEN) {
			int rem = cnt - (blocks - 1) * BASE_LEN;
			const u32 tp = [](int deg) { u32 res = 1; while (deg--) { res *= 10; } return res; }(rem);
			digits.back() %= tp;
		}
		shrink();
	}

	int length() const {
		return _len(digits);
	}

	int to_int() const {
		int res = 0;
		for (auto x : digits) {
			res = res * BASE + x;
		}
		if (neg) {
			res = -res;
		}
		return res;
	}

	long long to_long() const {
		long long res = 0;
		for (auto x : digits) {
			res = res * BASE + x;
		}
		if (neg) {
			res = -res;
		}
		return res;
	}

	friend std::ostream& operator <<(std::ostream& ostr, const BigInteger& num) {
		if (num.neg) {
			ostr << "-";
		}
		for (int i = (int)num.digits.size() - 1; i >= 0; --i) {
			if (i < (int)num.digits.size() - 1) {
				ostr << std::setfill('0') << std::setw(BASE_LEN);
			}
			ostr << num.digits[i];
		}
		if (num.digits.empty()) {
			ostr << "0";
		}
		return ostr;
	}

	friend std::istream& operator >>(std::istream& istr, BigInteger& num) {
		std::string s;
		istr >> s;
		num = BigInteger(s);
		return istr;
	}
};