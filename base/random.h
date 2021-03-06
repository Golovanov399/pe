#pragma once

#include <random>

mt19937 rr;
mt19937_64 rr64;

inline std::uint_fast32_t custom_rand() {
	return rr();
}

inline std::uint_fast32_t custom_rand64() {
	return rr64();
}

#define rand custom_rand
#define rand64 custom_rand64