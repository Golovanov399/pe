#pragma once

#include <vector>
#include "montgomery.h"
#include "ntt.h"
#include "fft_ll.h"
#include "fft_crt.h"

using std::vector;

template <typename T>
struct BaseMultiplicator {
	static void multiply_in_place(vector<T>& a, const vector<T>& b) {
		if (a.empty() || b.empty()) {
			a = {};
			return;
		}
		int b_deg = (int)b.size() - 1;
		a.resize(a.size() + b_deg);
		for (int i = (int)a.size() - 1 - b_deg; i >= 0; --i) {
			for (int j = b_deg; j > 0; --j) {
				a[i + j] += a[i] * b[j];
			}
			a[i] *= b[0];
		}
	}
};

template <typename T>
struct NormalMultiplicator : BaseMultiplicator<T> {
	static vector<T> multiply(const vector<T>& a, const vector<T>& b) {
		auto res = a;
		BaseMultiplicator<T>::multiply_in_place(res, b);
		return res;
	}
};

template <int mod, int N>
struct NttMultiplicator : BaseMultiplicator<Montgomery<mod>> {
	inline static NTT<mod, N> ntt;

	static vector<Montgomery<mod>> multiply(const vector<Montgomery<mod>>& a, const vector<Montgomery<mod>>& b) {
		return ntt.multiply(a, b);
	}
};

template <typename real_type, int N>
struct FftLlMultiplicator : BaseMultiplicator<long long> {
	static_assert(is_same_v<real_type, long double>);
	inline static FFTLL<real_type, N> fft;

	static vector<long long> multiply(const vector<long long>& a, const vector<long long>& b) {
		return fft.multiply(a, b);
	}
};

template <typename modulo_type, int N>
struct FftCrtMultiplicator : BaseMultiplicator<modulo_type> {
	inline static FFTCrt<modulo_type, N> fft;

	static vector<modulo_type> multiply(const vector<modulo_type>& a, const vector<modulo_type>& b) {
		return fft.multiply(a, b);
	}
};

enum class FftType {
	NoFft = 0,
	Ntt = 1,
	FftLl = 2,
	FftCrt = 3
};

struct fft_tag {};

struct no_fft_tag : fft_tag {
	static const FftType type = FftType::NoFft;
};

template <int N, FftType _type>
struct typed_fft_tag : fft_tag {
	static constexpr int size() {
		return N;
	}
	static const FftType type = _type;
};

template <int N> using ntt_tag = typed_fft_tag<N, FftType::Ntt>;
template <int N> using fft_ll_tag = typed_fft_tag<N, FftType::FftLl>;
template <int N> using fft_crt_tag = typed_fft_tag<N, FftType::FftCrt>;

template <typename T, typename tag> struct Multiplicator;
template <typename T> struct Multiplicator<T, no_fft_tag> : NormalMultiplicator<T> {};
template <typename T, int N> struct Multiplicator<T, ntt_tag<N>> : NttMultiplicator<T::mod(), N> {};
template <typename T, int N> struct Multiplicator<T, fft_ll_tag<N>> : FftLlMultiplicator<std::enable_if_t<is_same_v<T, long long>, long double>, N> {};
template <typename T, int N> struct Multiplicator<T, fft_crt_tag<N>> : FftCrtMultiplicator<T, N> {};