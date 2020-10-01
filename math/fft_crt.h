#pragma once

#include "../base.h"
#include "fft_interface.h"
#include "ntt.h"
#include "modular.h"
#include "crt.h"

template <typename modulo_type, int N>
class FFTCrt : public IFFT<modulo_type, modulo_type, N> {
	static constexpr int mod1 = 167772161;	// 2^25
	static constexpr int mod2 = 469762049;	// 2^26
	static constexpr int mod3 = 754974721;	// 2^24
	static constexpr LI modprod = (LI)mod1 * mod2 * mod3;
	using Mint1 = Modular<mod1>;
	using Mint2 = Modular<mod2>;
	using Mint3 = Modular<mod3>;
public:
	vector<modulo_type> multiply(vector<modulo_type> a, vector<modulo_type> b) {
		if (!this->initialized_) {
			this->initialize();
		}
		vector<Mint1> a1(a.size()), b1(b.size());
		vector<Mint2> a2(a.size()), b2(b.size());
		vector<Mint3> a3(a.size()), b3(b.size());
		for (int i = 0; i < (int)a.size(); ++i) {
			a1[i] = a[i].val;
			a2[i] = a[i].val;
			a3[i] = a[i].val;
		}
		for (int i = 0; i < (int)b.size(); ++i) {
			b1[i] = b[i].val;
			b2[i] = b[i].val;
			b3[i] = b[i].val;
		}
		auto r1 = ntt1.multiply(a1, b1);
		auto r2 = ntt2.multiply(a2, b2);
		auto r3 = ntt3.multiply(a3, b3);
		vector<modulo_type> res(r1.size());
		for (int i = 0; i < (int)res.size(); ++i) {
			res[i] = (c1 * r1[i].val + c2 * r2[i].val + c3 * r3[i].val) % modprod % modulo_type::mod();
		}
		return res;
	}

protected:
	LI c1 = crt(vector<Remainder<LI>>{{mod1, 1}, {mod2, 0}, {mod3, 0}}).rem;
	LI c2 = crt(vector<Remainder<LI>>{{mod1, 0}, {mod2, 1}, {mod3, 0}}).rem;
	LI c3 = crt(vector<Remainder<LI>>{{mod1, 0}, {mod2, 0}, {mod3, 1}}).rem;

	NTT<mod1, N> ntt1;
	NTT<mod2, N> ntt2;
	NTT<mod3, N> ntt3;

	void fill_angles() {}
};

// template <int mod, typename real_type, int N = (1 << 20)>
// using FFT = FFTMod<Modular<mod>, real_type, N>;