/*
----	Sam Warren 2019	----
3x3 Column-Major Affine Transformation Matrix + Translation Vector
*/
#pragma once
#include <array>
#include "vec3.h"

namespace maths {
	template<class T>
	constexpr std::array<T, 12> identityXfm = { 1,0,0, 0,1,0, 0,0,1, 0,0,0 };
}

template<
	typename T,
	typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
>
class affine3 {
	public:
		affine3() {
			xfm = maths::identityXfm<T>;
		}

		affine3(const T *_xfm) {
			memcpy(&xfm, _xfm, sizeof(T) * 12);
		}

		affine3(const vec3<T> &_a, const vec3<T> &_b, const vec3<T> &_c, const vec3<T> &_p = vec3<T>(0, 0, 0)) {
			memcpy(&xfm[0], &_a, sizeof(T) * 3);
			memcpy(&xfm[3], &_b, sizeof(T) * 3);
			memcpy(&xfm[6], &_c, sizeof(T) * 3);
			memcpy(&xfm[9], &_p, sizeof(T) * 3);
		}

		inline T operator[](const unsigned _rhs) const { return xfm[_rhs]; }
		inline T &operator[](const unsigned _rhs) { return xfm[_rhs]; }

		inline vec3<T> operator*(const vec3<T> &_rhs) const {
			return vec3<T>(&xfm[0]) * _rhs.x + vec3<T>(&xfm[3]) * _rhs.y + vec3<T>(&xfm[6]) * _rhs.z + vec3<T>(&xfm[9]);
		}

		inline vec3<T> TransformPoint(const vec3<T> &_rhs) const {
			return *this * _rhs;
		}

		inline vec3<T> TransformVector(const vec3<T> &_rhs) const {
			return vec3<T>(&xfm[0]) * _rhs.x + vec3<T>(&xfm[3]) * _rhs.y + vec3<T>(&xfm[6]) * _rhs.z;
		}

		inline vec3<T> TransformNormal(const vec3<T> &_rhs) const {
			return (vec3<T>(&xfm[0]) * _rhs.x + vec3<T>(&xfm[3]) * _rhs.y + vec3<T>(&xfm[6]) * _rhs.z).Normalised();
		}

		inline affine3<T> operator*(const affine3<T> &_rhs) const {
			const vec3<T> a(
				xfm[0] * _rhs.xfm[0] + xfm[1] * _rhs.xfm[3] + xfm[2] * _rhs.xfm[6],
				xfm[0] * _rhs.xfm[1] + xfm[1] * _rhs.xfm[4] + xfm[2] * _rhs.xfm[7],
				xfm[0] * _rhs.xfm[2] + xfm[1] * _rhs.xfm[5] + xfm[2] * _rhs.xfm[8]);
			const vec3<T> b(
				xfm[3] * _rhs.xfm[0] + xfm[4] * _rhs.xfm[3] + xfm[5] * _rhs.xfm[6],
				xfm[3] * _rhs.xfm[1] + xfm[4] * _rhs.xfm[4] + xfm[5] * _rhs.xfm[7],
				xfm[3] * _rhs.xfm[2] + xfm[4] * _rhs.xfm[5] + xfm[5] * _rhs.xfm[8]);
			const vec3<T> c(
				xfm[6] * _rhs.xfm[0] + xfm[7] * _rhs.xfm[3] + xfm[8] * _rhs.xfm[6],
				xfm[6] * _rhs.xfm[1] + xfm[7] * _rhs.xfm[4] + xfm[8] * _rhs.xfm[7],
				xfm[6] * _rhs.xfm[2] + xfm[7] * _rhs.xfm[5] + xfm[8] * _rhs.xfm[8]);
			const vec3<T> p(xfm[9] + _rhs.xfm[9], xfm[10] + _rhs.xfm[10], xfm[11] + _rhs.xfm[11]);
			return affine3<T>(a, b, c, p);
		}

		inline bool IsIdentity() const {
			return xfm == maths::identityXfm<T>;
		}

		static inline affine3<T> GetRotationX(const T _theta) {
			const T sinTheta = std::sin(_theta);
			const T cosTheta = std::sqrt(1 - sinTheta * sinTheta);
			return affine3<T>(vec3<T>(1, 0, 0), vec3<T>(0, cosTheta, -sinTheta), vec3<T>(0, sinTheta, cosTheta));
		}
		static inline affine3<T> GetRotationY(const T _theta) {
			const T sinTheta = std::sin(_theta);
			const T cosTheta = std::sqrt(1 - sinTheta * sinTheta);
			return affine3<T>(vec3<T>(cosTheta, 0, sinTheta), vec3<T>(0, 1, 0), vec3<T>(-sinTheta, 0, cosTheta));
		}
		static inline affine3<T> GetRotationZ(const T _theta) {
			const T sinTheta = std::sin(_theta);
			const T cosTheta = std::sqrt(1 - sinTheta * sinTheta);
			return affine3<T>(vec3<T>(cosTheta, -sinTheta, 0), vec3<T>(sinTheta, cosTheta, 0), vec3<T>(0, 0, 1));
		}

	private:
		std::array<T, 12> xfm;
};