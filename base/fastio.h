#pragma once

#include <cstring>

// taken from https://github.com/NyaanNyaan/library/blob/master/misc/fastio.hpp
namespace fastio {

static constexpr int SZ = 1 << 17;
char ibuf[SZ], obuf[SZ];
int pil = 0, pir = 0, por = 0;

struct Pre {
	char num[40000];
	constexpr Pre() : num() {
		for (int i = 0; i < 10000; i++) {
			int n = i;
			for (int j = 3; j >= 0; j--) {
				num[i * 4 + j] = n % 10 + '0';
				n /= 10;
			}
		}
	}
} constexpr pre;

inline void load() {
	memcpy(ibuf, ibuf + pil, pir - pil);
	pir = pir - pil + fread(ibuf + pir - pil, 1, SZ - pir + pil, stdin);
	pil = 0;
}

inline void flush() {
	fwrite(obuf, 1, por, stdout);
	por = 0;
}

inline void skip_space() {
	if (pil + 32 > pir) load();
	while (ibuf[pil] <= ' ') pil++;
}

inline void read(char& c) {
	if (pil + 32 > pir) load();
	c = ibuf[pil++];
}

template <typename T>
inline void read(T& x) {
	if (pil + 32 > pir) load();
	char c;
	do c = ibuf[pil++];
	while (c < '-');
	[[maybe_unused]] bool minus = false;
	if constexpr (std::is_signed<T>::value == true) {
		if (c == '-') minus = true, c = ibuf[pil++];
	}
	x = 0;
	while (c >= '0') {
		x = x * 10 + (c & 15);
		c = ibuf[pil++];
	}
	if constexpr (std::is_signed<T>::value == true) {
		if (minus) x = -x;
	}
}

inline void read() {}

template <typename Head, typename... Tail>
inline void read(Head& head, Tail&... tail) {
	read(head);
	read(tail...);
}

inline void write(char c) {
	if (por > SZ - 32) flush();
	obuf[por++] = c;
}

inline void write(bool b) { 
	if (por > SZ - 32) flush();
	obuf[por++] = b ? '1' : '0'; 
}

template <typename T>
inline void write(T x) {
	if (por > SZ - 32) flush();
	if (!x) {
		obuf[por++] = '0';
		return;
	}
	if constexpr (std::is_signed<T>::value == true) {
		if (x < 0) obuf[por++] = '-', x = -x;
	}
	int i = 12;
	char buf[16];
	while (x >= 10000) {
		memcpy(buf + i, pre.num + (x % 10000) * 4, 4);
		x /= 10000;
		i -= 4;
	}
	if (x < 100) {
		if (x < 10) {
			obuf[por] = '0' + x;
			++por;
		} else {
			uint32_t q = (uint32_t(x) * 205) >> 11;
			uint32_t r = uint32_t(x) - q * 10;
			obuf[por] = '0' + q;
			obuf[por + 1] = '0' + r;
			por += 2;
		}
	} else {
		if (x < 1000) {
			memcpy(obuf + por, pre.num + (x << 2) + 1, 3);
			por += 3;
		} else {
			memcpy(obuf + por, pre.num + (x << 2), 4);
			por += 4;
		}
	}
	memcpy(obuf + por, buf + i + 4, 12 - i);
	por += 12 - i;
}

inline void write() {}

template <typename Head, typename... Tail>
inline void write(Head&& head, Tail&&... tail) {
	write(head);
	write(forward<Tail>(tail)...);
}

template <typename... Args>
inline void writeln(Args&&... x) {
	write(forward<Args>(x)...);
	write('\n');
}

struct Dummy {
	Dummy() { atexit(flush); }
} dummy;


inline int nextInt() {
	int x;
	read(x);
	return x;
}

inline long long nextLong() {
	long long x;
	read(x);
	return x;
}

inline char nextChar() {
	char c;
	read(c);
	return c;
}

}  // namespace fastio