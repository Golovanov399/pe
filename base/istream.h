#pragma once

#include "base.h"

template <typename T>
istream& operator >>(istream& istr, vector<T>& vec) {
	for (auto& x : vec) {
		istr >> x;
	}
	return istr;
}

template <typename T, typename U>
istream& operator >>(istream& istr, pair<T, U>& pr) {
	return istr >> pr.first >> pr.second;
}

template <typename T = int>
T nxt(istream& istr = cin) {
	T x;
	istr >> x;
	return x;
}
