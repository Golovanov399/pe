#pragma once

template <uint32_t mod>
struct Montgomery {
	using i32 = int32_t;
	using u32 = uint32_t;
	using u64 = uint64_t;

	static constexpr u32 np = []() {
		u32 x = mod;
		for (int i = 0; i < 4; ++i) {
			x *= 2u - mod * x;
		}
		return -x;
	}();
	static constexpr u32 r2 = -(u64)(mod) % mod;

	static_assert(mod < (1u << 30));
	static_assert(mod * np + 1 == 0);

	static u32 reduce(u64 x) {
		return (x + (u64)((u32)x * np) * mod) >> 32;
	}

	u32 x;
	Montgomery(): x(0) {}
	Montgomery(long long y): x(reduce((u64)(y % mod + mod) * r2)) {}

	Montgomery& operator +=(const Montgomery& ot) {
		if ((i32)(x += ot.x - 2 * mod) < 0) {
			x += 2 * mod;
		}
		return *this;
	}

	Montgomery& operator -=(const Montgomery& ot) {
		if ((i32)(x -= ot.x) < 0) {
			x += 2 * mod;
		}
		return *this;
	}

	Montgomery& operator *=(const Montgomery& ot) {
		x = reduce((u64)x * ot.x);
		return *this;
	}

	Montgomery& operator /=(const Montgomery& ot) {
		return *this *= ot.inverse();
	}

	Montgomery operator +(const Montgomery& ot) const {
		Montgomery res = *this;
		res += ot;
		return res;
	}

	Montgomery operator -(const Montgomery& ot) const {
		Montgomery res = *this;
		res -= ot;
		return res;
	}

	Montgomery operator *(const Montgomery& ot) const {
		Montgomery res = *this;
		res *= ot;
		return res;
	}

	Montgomery operator /(const Montgomery& ot) const {
		Montgomery res = *this;
		res /= ot;
		return res;
	}

	Montgomery operator -() const {
		return Montgomery() - *this;
	}

	u32 get() const {
		u32 res = reduce(x);
		return res < mod ? res : res - mod;
	}

	Montgomery inverse() const {
		return pow(mod - 2);
	}

	Montgomery pow(int64_t p) const {
		if (p < 0) {
			return pow(-p).inverse();
		}
		Montgomery res = 1;
		Montgomery a = *this;
		while (p) {
			if (p & 1) {
				res *= a;
			}
			p >>= 1;
			a *= a;
		}
		return res;
	}

	friend istream& operator >>(istream& istr, Montgomery& m) {
		long long x;
		istr >> x;
		m = Montgomery(x);
		return istr;
	}

	friend ostream& operator <<(ostream& ostr, const Montgomery& m) {
		return ostr << m.get();
	}

	explicit operator int64_t() const {
		return x;
	}

	explicit operator bool() const {
		return x;
	}
};

