#pragma once

#include <cassert>
#include <string>

#include "polynomial.h"

using std::string;

template <typename T, typename tag>
struct PolynomialParser {
	using Poly = Polynomial<T, tag>;
	string s;
	int i;

	explicit PolynomialParser(const string& _s): s(""), i(0) {
		for (char c : _s) {
			if (!isspace(c)) {
				s += c;
			}
		}
	}

	Poly parse_expr() {
		Poly p = parse_atom();
		while (i < (int)s.length()) {
			if (s[i] != '+' && s[i] != '-') {
				break;
			}
			char c = parse_pm_sign();
			auto q = parse_atom();
			if (c == '+') {
				p += q;
			} else {
				p -= q;
			}
		}
		return p;
	}

	Poly parse_atom() {
		Poly p(vector<T>{1});
		Poly last = parse_term();
		char op = '*';
		while (i < (int)s.length()) {
			if (s[i] == ')' || s[i] == '}') {
				break;
			}
			if (s[i] == '^' || (i < (int)s.length() - 1 && s[i] == '*' && s[i + 1] == '*')) {
				parse_pow_sign();
				auto deg = parse_num();
				last = last.pow(deg);
			} else if (s[i] == '*' || s[i] == '/') {
				if (op == '*') {
					p *= last;
				} else {
					p /= last;
				}
				op = parse_md_sign();
				last = parse_term();
			} else if (s[i] != '+' && s[i] != '-') {
				last *= parse_term();
			} else {
				break;
			}
		}
		if (op == '*') {
			p *= last;
		} else {
			p /= last;
		}
		return p;
	}

	Poly parse_term() {
		if (i == (int)s.length()) {
			assert(false);
		}
		if (s[i] == '(') {
			++i;
			auto res = parse_expr();
			assert(i < (int)s.length() && s[i] == ')');
			++i;
			return res;
		}
		if (s[i] == '{') {
			++i;
			auto res = parse_expr();
			assert(i < (int)s.length() && s[i] == '}');
			++i;
			return res;
		}
		if (s[i] == 'x' || s[i] == 't') {
			return parse_var();
		}
		return Poly{vector<T>{parse_num()}};
	}

	Poly parse_var() {
		if (i < (int)s.length() && (s[i] == 'x' || s[i] == 't')) {
			++i;
			return Poly({0, 1});
		} else {
			assert(false);
		}
	}

	T parse_num() {
		int sign = 1;
		if (i < (int)s.length() && s[i] == '-') {
			sign = -1;
			++i;
		}
		return parse_nat() * sign;
	}

	T parse_nat() {
		int cnt = 0;
		T res = 0;
		while (i < (int)s.length() && isdigit(s[i])) {
			++cnt;
			res = res * 10 + parse_digit() - '0';
		}
		assert(cnt > 0);
		return res;
	}

	char parse_digit() {
		if (i < (int)s.length() && isdigit(s[i])) {
			return s[i++];
		} else {
			assert(false);
		}
	}

	char parse_pm_sign() {
		if (i < (int)s.length() && s[i] == '+') {
			++i;
			return '+';
		}
		if (i < (int)s.length() && s[i] == '-') {
			++i;
			return '-';
		}
		assert(false);
	}

	char parse_md_sign() {
		if (i < (int)s.length() && s[i] == '*') {
			++i;
			return '*';
		}
		if (i < (int)s.length() && s[i] == '/') {
			++i;
			return '/';
		}
		assert(false);
	}

	void parse_pow_sign() {
		if (i < (int)s.length() && s[i] == '^') {
			++i;
		} else if (i < (int)s.length() - 1 && s[i] == '*' && s[i + 1] == '*') {
			i += 2;
		} else {
			assert(false);
		}
	}
};

template <typename Poly>
Poly parse_poly(const string& s) {
	return PolynomialParser<typename Poly::Type, typename Poly::Tag>(s).parse_expr();
}
