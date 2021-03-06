//----	By Sam Warren 2019	----
//----	Template class for 2-dimensional vectors of any numerical type. Includes basic operators and utilities.	----

#pragma once
#include <type_traits>

template<
	typename T,
	typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
>
class vec2 {
public:
	T x, y;

	vec2(const T _x = 0, const T _y = 0) {
		x = _x;
		y = _y;
	}

	inline vec2<T> operator+(const vec2<T> &_rhs) const { return vec2<T>(x + _rhs.x, y + _rhs.y); }
	inline vec2<T> operator-(const vec2<T> &_rhs) const { return vec2<T>(x - _rhs.x, y - _rhs.y); }
	inline vec2<T> operator*(const vec2<T> &_rhs) const { return vec2<T>(x * _rhs.x, y * _rhs.y); }
	inline vec2<T> operator*(const T _rhs) const { return vec2<T>(x * _rhs, y * _rhs); }
	inline vec2<T> operator/(const vec2<T> &_rhs) const { return vec2<T>(x / _rhs.x, y / _rhs.y); }
	inline vec2<T> operator/(const T _rhs) const { const T inv = (T)1 / _rhs; return vec2<T>(x * inv, y * inv); }

	inline void operator+=(const vec2<T> &_rhs) {
		x += _rhs.x;
		y += _rhs.y;
	}
	inline void operator-=(const vec2<T> &_rhs) {
		x -= _rhs.x;
		y -= _rhs.y;
	}
	inline void operator*=(const vec2<T> &_rhs) {
		x *= _rhs.x;
		y *= _rhs.y;
	}
	inline void operator*=(const T _rhs) {
		x *= _rhs;
		y *= _rhs;
	}
	inline void operator/=(const vec2<T> &_rhs) {
		x /= _rhs.x;
		y /= _rhs.y;
	}
	inline void operator/=(const T _rhs) {
		const T inv = (T)1 / _rhs;
		x *= inv;
		y *= inv;
	}

	inline bool operator==(const vec2<T> &_rhs) const { return x == _rhs.x && y == _rhs.y; }
	inline bool operator!=(const vec2<T> &_rhs) const { return x != _rhs.x || y != _rhs.y; }

	inline vec2<T> Normalised() const {
		const T inv = (T)1 / Magnitude();
		return *this * inv;
	}

	inline T Magnitude() const { return std::sqrt(x*x + y*y); }

	inline vec2<T> Mod(const T _q = 1) const {
		return vec2<T>(std::fmod(x, _q), std::fmod(y, _q));
	}

	inline vec2<T> Wrap(const T _xb = 1, const T _yb = 1) const {
		return vec2<T>(x - _xb * std::floor(x / _xb), y - _yb * std::floor(y / _yb));
	}

};

namespace maths {

	template<class T>
	inline T Dot(const vec2<T> &_a, const vec2<T> &_b) {
		return _a.x*_b.x + _a.y*_b.y;
	}

	template<class T>
	inline vec2<T> Mod(const vec2<T> &_vec, const T _q = 1) {
		return vec2<T>(std::fmod(_vec.x, _q), std::fmod(_vec.y, _q));
	}

}