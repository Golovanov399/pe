#pragma once

#include <iostream>
#include <vector>
#include <utility>

using std::istream;

template <typename T>
istream& operator >>(istream& istr, std::vector<T>& vec) {
	for (auto& x : vec) {
		istr >> x;
	}
	return istr;
}

template <typename T, typename U>
istream& operator >>(istream& istr, std::pair<T, U>& pr) {
	return istr >> pr.first >> pr.second;
}

template <typename T = int>
T nxt(istream& istr = std::cin) {
	T x;
	istr >> x;
	return x;
}
