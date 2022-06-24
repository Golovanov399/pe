#pragma once

#include <vector>

#include "../base/ostream.h"
#include "poly_multiplicator.h"

using std::vector;

template <typename T, typename tag = no_fft_tag>
struct Polynomial {
	vector<T> coeff;

	Polynomial() {}
	explicit Polynomial(int sz): coeff(sz) {}
	Polynomial(const vector<T>& vec): coeff(vec) {}

	Polynomial& operator +=(const Polynomial& ot) {
		extend_to(ot.deg());
		for (int i = 0; i < (int)ot.size(); ++i) {
			coeff[i] += ot[i];
		}
		shrink();
		return *this;
	}

	Polynomial& operator -=(const Polynomial& ot) {
		extend_to(ot.deg());
		for (int i = 0; i < (int)ot.size(); ++i) {
			coeff[i] -= ot[i];
		}
		shrink();
		return *this;
	}

	Polynomial& operator *=(const Polynomial& ot) {
		coeff = Multiplicator<T, tag>::multiply(coeff, ot.coeff);
		return *this;
	}

	Polynomial& operator *=(const T& ot) {
		for (auto& x : coeff) {
			x *= ot;
		}
		return *this;
	}

	Polynomial& operator /=(const T& ot) {
		auto i = (T)1 / ot;
		for (auto& x : coeff) {
			x *= i;
		}
		return *this;
	}

	Polynomial& operator <<=(int deg) {
		extend_by(deg);
		for (int i = (int)coeff.size() - 1; i >= deg; --i) {
			coeff[i] = coeff[i - deg];
		}
		std::fill_n(coeff.begin(), deg, 0);
		return *this;
	}

	Polynomial& operator >>=(int deg) {
		if ((int)coeff.size() <= deg) {
			coeff = {};
			return *this;
		}
		coeff.erase(coeff.begin(), coeff.begin() + deg);
		return *this;
	}

	friend Polynomial operator +(Polynomial a, const Polynomial& b) {
		return a += b;
	}

	friend Polynomial operator -(Polynomial a, const Polynomial& b) {
		return a -= b;
	}

	friend Polynomial operator *(Polynomial a, const Polynomial& b) {
		return a *= b;
	}

	friend Polynomial operator *(Polynomial a, const T& b) {
		return a *= b;
	}

	friend Polynomial operator /(Polynomial a, const T& b) {
		return a /= b;
	}

	friend Polynomial operator <<(Polynomial a, int deg) {
		return a <<= deg;
	}

	friend Polynomial operator >>(Polynomial a, int deg) {
		return a >>= deg;
	}

	Polynomial pow(int pw) const {
		// TODO: implement it in the multiplicator
		Polynomial res(vector<T>{1});
		Polynomial cur = *this;
		while (pw) {
			if (pw & 1) {
				res *= cur;
			}
			pw >>= 1;
			cur *= cur;
		}
		return res;
	}

	Polynomial& operator /=(const Polynomial& p) {
		// TODO: implement it in the multiplicator
		assert(p.size() == 1);
		for (auto& x : coeff) {
			x /= p[0];
		}
		return *this;
	}

	friend Polynomial operator /(Polynomial a, const Polynomial& b) {
		return a /= b;
	}

	bool empty() const {
		return coeff.empty();
	}

	int size() const {
		return coeff.size();
	}

	int deg() const {
		return size() - 1;
	}

	void trim(int d) {
		if (deg() > d) {
			coeff.resize(d + 1);
		}
	}

	void extend_by(int d) {
		d += coeff.size();
		coeff.resize(d);
	}

	void extend_to(int d) {
		if (deg() < d) {
			coeff.resize(d + 1, 0);
		}
	}

	void shrink() {
		while (!coeff.empty() && coeff.back() == 0) {
			coeff.pop_back();
		}
	}

	const T& operator [](int idx) const {
		return coeff[idx];
	}

	T& operator [](int idx) {
		return coeff[idx];
	}

	T operator ()(const T& x) const {
		T res = 0;
		for (int i = deg(); i >= 0; --i) {
			res = res * x + coeff[i];
		}
		return res;
	}

	template <typename new_tag>
	Polynomial<T, new_tag> change_tag() const {
		return Polynomial<T, new_tag>(coeff);
	}
};

template <typename T, typename tag>
std::ostream& operator <<(std::ostream& ostr, const Polynomial<T, tag>& p) {
	return ostr << p.coeff;
}
