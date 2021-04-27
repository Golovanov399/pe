#pragma once

#include "../base/base.h"

template <typename T>
void and_wh(T b, T e, bool inverse = false) {
	if (e == b + 1) {
		return;
	}
	auto m = b + (e - b) / 2;
	and_wh(b, m);
	and_wh(m, e);
	while (m != e) {
		inverse ? *b -= *m : *b += *m;
		++b, ++m;
	}
}

template <typename T>
void or_wh(T b, T e, bool inverse = false) {
	if (e == b + 1) {
		return;
	}
	auto m = b + (e - b) / 2;
	or_wh(b, m);
	or_wh(m, e);
	while (m != e) {
		inverse ? *m -= *b : *m += *b;
		++b, ++m;
	}
}

template <typename T>
void xor_wh(T b, T e) {
	if (e == b + 1) {
		return;
	}
	auto m = b + (e - b) / 2;
	xor_wh(b, m);
	xor_wh(m, e);
	while (m != e) {
		auto x = *b, y = *m;
		*b = x + y, *m = x - y;
		++b, ++m;
	}
}

