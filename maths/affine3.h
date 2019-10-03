#pragma once
#include <array>
#include "vec3.h"

template<
	typename T,
	typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
>
class affine3 {
	public:
		affine3() {
			xfm = identityXfm;
		}

		inline T operator[](const unsigned _rhs) const { return xfm[_rhs]; }
		inline T &operator[](const unsigned _rhs) { return xfm[_rhs]; }

		inline vec3<T> operator*(const vec3<T> &_rhs) const {
			return vec3<T>(&xfm[0]) * _rhs.x + vec3<T>(&xfm[3]) * _rhs.y + vec3<T>(&xfm[6]) * _rhs.z;
		}

	protected:
		std::array<T, 12> xfm;	//3x4 Column-Major Affine Transformation + Translation
		const std::array<T, 12> identityXfm = { 1,0,0, 0,1,0, 0,0,1, 0,0,0 };
};