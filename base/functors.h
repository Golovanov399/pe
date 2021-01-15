#pragma once

#include "base.h"

#define CREATE_FUNCTOR(name, expr) \
template <typename T>\
struct name {\
	constexpr T operator ()(const T& x, const T& y) const { return expr; }\
}

CREATE_FUNCTOR(Min, min(x, y));
CREATE_FUNCTOR(Max, max(x, y));
CREATE_FUNCTOR(Sum, x + y);	// there already is std::plus
CREATE_FUNCTOR(Prod, x * y);	// there already is std::multiplies
// for more refer to https://en.cppreference.com/w/cpp/header/functional
