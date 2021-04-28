#pragma once

#include "base.h"

enum PrintModifier : int {
	Reset = 0,
	Bold = 1,
	Underline = 2,
	Inverse = 7,
	BoldOff = 21,
	UnderlineOff = 24,
	InverseOff = 27,
	FgBlack = 30,
	FgRed = 31,
	FgGreen = 32,
	FgYellow = 33,
	FgBlue = 34,
	FgMagenta = 35,
	FgCyan = 36,
	FgWhite = 37,
	BgBlack = 30,
	BgRed = 31,
	BgGreen = 32,
	BgYellow = 33,
	BgBlue = 34,
	BgMagenta = 35,
	BgCyan = 36,
	BgWhite = 37
};

namespace Emphasize {
	const vector<PrintModifier> Red = {PrintModifier::Bold, PrintModifier::FgRed};
	const vector<PrintModifier> Green = {PrintModifier::Bold, PrintModifier::FgGreen};
	const vector<PrintModifier> Yellow = {PrintModifier::Bold, PrintModifier::FgYellow};
	const vector<PrintModifier> Blue = {PrintModifier::Bold, PrintModifier::FgBlue};
	const vector<PrintModifier> Magenta = {PrintModifier::Bold, PrintModifier::FgMagenta};
	const vector<PrintModifier> Cyan = {PrintModifier::Bold, PrintModifier::FgCyan};
	const vector<PrintModifier> White = {PrintModifier::Bold, PrintModifier::FgWhite};
}  // namespace Emphasize

using namespace Emphasize;

struct debug {
	ostringstream ss;

	debug() {}
	explicit debug(PrintModifier pm) {
		ss << "\033[" << pm << "m";
	}
	explicit debug(const vector<PrintModifier>& pms) {
		ss << "\033[";
		for (int i = 0; i < (int)pms.size(); ++i) {
			if (i) {
				ss << ";";
			}
			ss << pms[i];
		}
		ss << "m";
	}

	~debug() {
		cerr << ss.str();
		cerr << "\033[" << PrintModifier::Reset << "m";
	}

	template <typename T>
	debug& operator <<(const T& x) {
		ss << x;
		return *this;
	}
};