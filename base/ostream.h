#pragma once

#include <array>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <set>
#include <tuple>
#include <utility>
#include <vector>
#include "traits.h"

using std::ostream, std::cout;
using std::make_tuple;

ostream& operator <<(ostream& ostr, LI x) {
	static constexpr li BIG = 1e18;
	if (x < 0) {
		ostr << "-";
		x = -x;
	}
	if (x < BIG) {
		return ostr << (li)x;
	} else if (x / BIG >= BIG) {
		std::stringstream ss;
		ss << std::setfill('0') << std::setw(18) << (li)(x / BIG % BIG);
		ss << std::setfill('0') << std::setw(18) << (li)(x % BIG);
		return ostr << (li)(x / BIG / BIG) << ss.str();
	} else {
		std::stringstream ss;
		ss << std::setfill('0') << std::setw(18) << (li)(x % BIG);
		return ostr << (li)(x / BIG) << ss.str();
	}
}

template <typename T>
ostream& operator <<(ostream& ostr, const std::vector<T>& vec) {
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

template <typename T, size_t N>
ostream& operator <<(ostream& ostr, const std::array<T, N>& vec) {
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
ostream& operator <<(ostream& ostr, const std::set<T>& st) {
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
ostream& operator <<(ostream& ostr, const std::multiset<T>& st) {
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
ostream& operator <<(ostream& ostr, const std::map<T, U>& mp) {
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
ostream& operator <<(ostream& ostr, const std::pair<T, U>& p) {
	return ostr << "(" << p.first << ", " << p.second << ")";
}

