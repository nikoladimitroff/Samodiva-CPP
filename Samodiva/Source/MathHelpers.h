#pragma once

namespace Samodiva
{
	template <typename T>
	int sign(T val) {
		return (T(0) < val) - (val < T(0));
	}

	template <typename T>
	int nonnegative_sign(T val) {
		return sign(val) < 0 ? 0 : sign(val);
	}
}