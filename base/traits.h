#pragma once

#include <type_traits>
#include <cstdint>

using li = long long;
using LI = __int128_t;
using ld = long double;
using uint = unsigned int;
using ull = unsigned long long;

template <typename, typename D = void> struct next_size;
template <typename T> using next_size_t = typename next_size<T>::type;
template <typename T> struct tag { using type = T; };

template <> struct next_size<int8_t> : tag<int16_t> {};
template <> struct next_size<int16_t> : tag<int32_t> {};
template <> struct next_size<int32_t> : tag<int64_t> {};
template <typename T> struct next_size<T, std::enable_if_t<4 < sizeof(T) && sizeof(T) <= 8, void>> : tag<__int128_t> {};
