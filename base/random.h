#pragma once

#include <random>

using std::mt19937, std::mt19937_64;
using std::shuffle;

mt19937 rr;
mt19937_64 rr64;

inline std::uint_fast32_t custom_rand() {
	return rr();
}

inline std::uint_fast64_t custom_rand64() {
	return rr64();
}

template <typename RandomIt>
inline void custom_random_shuffle(RandomIt begin, RandomIt end) {
	shuffle(begin, end, rr);
}

#define rand custom_rand
#define rand64 custom_rand64
#define random_shuffle custom_random_shuffle