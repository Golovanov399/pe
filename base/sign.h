#pragma once

#include "traits.h"

ld eps = 1e-8;
void set_eps(ld new_eps) {
	eps = new_eps;
}

template <typename T>
std::enable_if_t<std::is_integral_v<T>, int> sign(T x) {
	return x < 0 ? -1 : x > 0;
}

template <typename T>
std::enable_if_t<std::is_floating_point_v<T>, int> sign(T x) {
	return x < -eps ? -1 : x > eps;
}
