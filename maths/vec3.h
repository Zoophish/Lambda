//----	By Sam Warren 2019	----
//----	Template class for 3-dimensional vectors of any numerical type. Includes basic operators utilities.	----

#pragma once
#include <type_traits>

template<
	typename T,
	typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
>
class vec3 {
	public:
		T x, y, z;

		vec3(const T _x = 0, const T _y = 0, const T _z = 0) {
			x = _x;
			y = _y;
			z = _z;
		}

		inline operator T() const { return *this; }

		inline void operator=(const vec3<T> &_rhs) {
			x = _rhs.x;
			y = _rhs.y;
			z = _rhs.z;
		}

		inline vec3<T> operator+(const vec3<T> &_rhs) const { return vec3<T>(x + _rhs.x, y + _rhs.y, z + _rhs.z); }
		inline vec3<T> operator-(const vec3<T> &_rhs) const { return vec3<T>(x - _rhs.x, y - _rhs.y, z - _rhs.z); }
		inline vec3<T> operator*(const vec3<T> &_rhs) const { return vec3<T>(x * _rhs.x, y * _rhs.y, z * _rhs.z); }
		inline vec3<T> operator*(const T _rhs) const { return vec3<T>(x * _rhs, y * _rhs, z * _rhs); }
		inline vec3<T> operator/(const vec3<T> &_rhs) const { return vec3<T>(x / _rhs.x, y / _rhs.y, z / _rhs.z); }
		inline vec3<T> operator/(const T _rhs) const { const T inv = (T)1 / _rhs; return vec3<T>(x * inv, y * inv, z * inv); }

		inline void operator+=(const vec3<T> &_rhs) {
			x += _rhs.x;
			y += _rhs.y;
			z += _rhs.z;
		}
		inline void operator-=(const vec3<T> &_rhs) {
			x -= _rhs.x;
			y -= _rhs.y;
			z -= _rhs.z;
		}
		inline void operator*=(const vec3<T> &_rhs) {
			x *= _rhs.x;
			y *= _rhs.y;
			z *= _rhs.z;
		}
		inline void operator*=(const T _rhs) {
			x *= _rhs;
			y *= _rhs;
			z *= _rhs;
		}
		inline void operator/=(const vec3<T> &_rhs) {
			x /= _rhs.x;
			y /= _rhs.y;
			z /= _rhs.z;
		}
		inline void operator/=(const T _rhs) {
			const T inv = (T)1 / _rhs;
			x *= inv;
			y *= inv;
			z *= inv;
		}
		
		inline bool operator==(const vec3<T> &_rhs) const { return x == _rhs.x && y == _rhs.y && z == _rhs.z; }
		inline bool operator!=(const vec3<T> &_rhs) const { return x != _rhs.x || y != _rhs.y || z != _rhs.z; }

		inline vec3<T> Normalised() const {
			const T inv = (T)1 / Magnitude();
			return *this * inv;
		}

		inline T Magnitude() const { return std::sqrt(x*x + y*y + z*z); }

		static T Dot(const vec3<T> &_a, const vec3<T> &_b) { return _a.x*_b.x + _a.y*_b.y + _a.z*_b.z; }

		static vec3<T> Cross(const vec3<T> &_a, const vec3<T> &_b) { return vec3<T>(_a.y*_b.z - _a.z*_b.y, _a.z*_b.x - _a.x*_b.z, _a.x*_b.y - _a.y*_b.x); }
};