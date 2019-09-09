#pragma once
#include <array>

template<
	typename T,
	typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
>
class affine3 {
	public:
		static const affine3 identity;

		affine3() {
			xfm = identityXfm;
		}

		inline T operator[](const unsigned _rhs) const { return xfm[_rhs]; }
		inline T &operator[](const unsigned _rhs) { return xfm[_rhs]; }

	protected:
		std::array<T, 12> xfm;	//3x4 Row-Major
		const std::array<T, 12> identityXfm = { 1,0,0, 0,1,0, 0,0,1, 0,0,0 };
};