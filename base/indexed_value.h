#pragma once

#include <tuple>

template <typename T>
struct IndexedValue {
	int index;
	T value;

	bool operator <(const IndexedValue<T>& ot) const {
		return std::tie(value, index) < std::tie(ot.value, ot.index);
	}
};

template <typename T>
std::ostream& operator <<(std::ostream& ostr, const IndexedValue<T>& x) {
	return ostr << "(index=" << x.index << ", value=" << x.value << ")";
}

template <typename T>
std::vector<IndexedValue<T>> with_index(const std::vector<T>& vec) {
	std::vector<IndexedValue<T>> result(vec.size());
	for (int i = 0; i < (int)vec.size(); ++i) {
		result[i] = {i, vec[i]};
	}
	return result;
}