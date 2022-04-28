#pragma once

#include <type_traits>

#include "biginteger.h"

template <int prec>
struct FixedPrecision {
	BigInteger x;
	using Fp = FixedPrecision<prec>;

	FixedPrecision() {}
	template <typename T>
	FixedPrecision(T y) requires(std::is_integral_v<T> || std::is_floating_point_v<T>) {
		if constexpr (std::is_integral_v<T>) {
			x = BigInteger(y);
			x.add_zeroes(prec);
		} else if constexpr (std::is_floating_point_v<T>) {
			x.neg = y < 0;
			y = y < 0 ? -y : y;
			x = BigInteger(y);
			y -= floorl(y);
			x.add_zeroes(prec);
			BigInteger::u32 cur = 0;
			for (int i = prec - 1; i >= 0; --i) {
				y *= 10;
				BigInteger::u32 c = floorl(y);
				cur = cur * 10 + c;
				y -= c;
				if (i % BigInteger::BASE_LEN == 0) {
					x.digits[i / BigInteger::BASE_LEN] += cur;
					cur = 0;
				}
			}
		} else {
			assert(false);
		}
	}
	FixedPrecision(const std::string& s) {
		if (auto pos = s.find('.'); pos == std::string::npos) {
			x = BigInteger(s + std::string(prec, '0'));
		} else {
			int len_after = std::min<int>((int)s.length() - 1 - pos, prec);
			x = BigInteger(s.substr(0, pos) + s.substr(pos + 1, len_after) + std::string(prec - len_after, '0'));
		}
	}

	Fp& operator +=(const Fp& ot) {
		x += ot.x;
		return *this;
	}

	Fp& operator -=(const Fp& ot) {
		x -= ot.x;
		return *this;
	}

	Fp& operator *=(const Fp& ot) {
		x *= ot.x;
		x.drop_digits(prec);
		return *this;
	}

	Fp& operator /=(const Fp& ot) {
		x.add_zeroes(prec);
		x /= ot.x;
		return *this;
	}

	friend Fp operator +(Fp a, const Fp& b) {
		return a += b;
	}

	friend Fp operator -(Fp a, const Fp& b) {
		return a -= b;
	}

	friend Fp operator *(Fp a, const Fp& b) {
		return a *= b;
	}

	friend Fp operator /(Fp a, const Fp& b) {
		return a /= b;
	}

	Fp pow(uint64_t x) const {
		if (!x) {
			return 1;
		}
		Fp res = 1;
		for (int i = 63 - __builtin_clzll(x); i >= 0; --i) {
			std::cerr << res << "\n";
			res *= res;
			if ((x >> i) & 1) {
				res *= *this;
			}
		}
		return res;
	}

	friend std::ostream& operator <<(std::ostream& ostr, const Fp& num) {
		auto tmp = num.x;
		tmp.drop_digits(prec);
		ostr << tmp << ".";
		tmp = num.x;
		tmp.leave_digits(prec);
		tmp.neg = false;
		int cnt_zeroes = prec - tmp.length();
		ostr << std::string(cnt_zeroes, '0');
		return ostr << tmp;
	}

	friend std::istream& operator >>(std::istream& istr, Fp& num) {
		std::string s;
		istr >> s;
		num = Fp(s);
		return istr;
	}
};

using BigDecimal = FixedPrecision<36>;