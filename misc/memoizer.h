#pragma once

#include <map>
#include <vector>
#include <type_traits>

using std::map, std::unordered_map, std::vector;
using std::is_same_v, std::is_integral_v, std::decay_t;

template <typename S, typename T, typename Container, typename Fun>
class GenericMemoizerResult {
public:
	template <typename F>
	explicit GenericMemoizerResult(F&& _f): f(forward<F>(_f)), cache({}) {}

	T operator ()(const S& x) {
		if constexpr (is_same_v<Container, map<S, T>> || is_same_v<Container, unordered_map<S, T>>) {
			if (auto it = cache.find(x); it != cache.end()) {
				return it->second;
			}
		} else {
			// TODO: replace by requiring find()
			if (cache.count(x)) {
				return cache[x];
			}
		}
		auto res = f(ref(*this), x);
		return cache[x] = res;
	}

private:
	Fun f;
	Container cache;
};

template <typename S, typename T, typename Container = map<S, T>, typename Fun = void>
decltype(auto) GenericMemoizer(Fun&& fun) {
	return GenericMemoizerResult<S, T, Container, decay_t<Fun>>(forward<Fun>(fun));
}


template <typename S, typename T, typename Container, typename Fun>
class MemoizerResult {
public:
	template <typename F>
	explicit MemoizerResult(F&& _f, size_t size = 1000000): f(forward<F>(_f)), small_cache(size), calculated(size, false), cache({}) {}

	static_assert(is_integral_v<S>);

	T operator ()(S x) {
		if (x < static_cast<S>(small_cache.size())) {
			if (calculated[x]) {
				return small_cache[x];
			}
		} else if constexpr (is_same_v<Container, map<S, T>> || is_same_v<Container, unordered_map<S, T>>) {
			if (auto it = cache.find(x); it != cache.end()) {
				return it->second;
			}
		} else {
			// TODO: replace by requiring find()
			if (cache.count(x)) {
				return cache[x];
			}
		}
		auto res = f(ref(*this), x);
		if (x < static_cast<S>(small_cache.size())) {
			calculated[x] = true;
			small_cache[x] = res;
			return res;
		} else {
			return cache[x] = res;
		}
	}

private:
	Fun f;
	vector<T> small_cache;
	vector<bool> calculated;
	Container cache;
};

template <typename S, typename T, typename Container = map<S, T>, typename Fun = void>
decltype(auto) Memoizer(Fun&& fun, size_t size = 1000000) {
	return MemoizerResult<S, T, Container, decay_t<Fun>>(forward<Fun>(fun), size);
}