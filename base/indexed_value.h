#pragma once

template <typename T>
struct IndexedValue {
	int index;
	T value;

	bool operator <(const IndexedValue<T>& ot) const {
		return tie(value, index) < tie(ot.value, ot.index);
	}
};

template <typename T>
ostream& operator <<(ostream& ostr, const IndexedValue<T>& x) {
	return ostr << "(index=" << x.index << ", value=" << x.value << ")";
}

template <typename T>
vector<IndexedValue<T>> with_index(const vector<T>& vec) {
	vector<IndexedValue<T>> result(vec.size());
	for (int i = 0; i < (int)vec.size(); ++i) {
		result[i] = {i, vec[i]};
	}
	return result;
}