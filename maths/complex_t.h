//----	By Sam Warren 2019	----
//----	Template for complex numbers with a constituent real and imaginary part of any numerical type. Includes basic operators.	----

#pragma once
#include <type_traits>

template<
	typename T,
	typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
>
class complex_t {
	public:
		T r, i;

		complex_t(const T _r = 0, const T _i = 0) {
			r = _r;
			i = _i;
		}

		inline complex_t<T> operator+(const complex_t<T> &_rhs) const {
			return complex_t<T>(r + _rhs.r, i + _rhs.i);
		}
		inline complex_t<T> operator-(const complex_t<T> &_rhs) const {
			return complex_t<T>(r - _rhs.r, i - _rhs.i);
		}
		inline complex_t<T> operator*(const complex_t<T> &_rhs) const {
			return complex_t<T>(r * _rhs.r - i * _rhs.i, r * _rhs.i + i * _rhs.r);
		}
		inline complex_t<T> operator*(const T _rhs) const {
			return complex_t<T>(r * _rhs, i * _rhs);
		}
		inline complex_t<T> operator/(const complex_t<T> &_rhs) const {
			const T invR = (T)1 / _rhs.r;
			const T invI = (T)1 / _rhs.i;
			return complex_t<T>(r * invR - i * invI, r * invI + i * invR);
		}
		inline complex_t<T> operator/(const T _rhs) const {
			const T inv = (T)1 / _rhs;
			return complex_t<T>(r * inv, i * inv);
		}

		inline void operator+=(const complex_t<T> &_rhs) {
			r += _rhs.r;
			i += _rhs.i;
		}
		inline void operator-=(const complex_t<T> &_rhs) {
			r -= _rhs.r;
			i -= _rhs.i;
		}
		inline void operator*=(const complex_t<T> &_rhs) {
			r = r * _rhs.r - i * _rhs.i;
			i = r * _rhs.i + i * _rhs.r;
		}
		inline void operator*=(const T _rhs) {
			r *= _rhs;
			i *= _rhs;
		}
		inline void operator/=(const complex_t<T> &_rhs) {
			const T invR = (T)1 / _rhs.r;
			const T invI = (T)1 / _rhs.i;
			r = r * invR - i * invI;
			i = r * invI + i * invR;
		}
		inline void operator/=(const T _rhs) {
			const T inv = 1 / _rhs;
			r *= inv;
			i *= inv;
		}

		inline bool operator==(const complex_t<T> &_rhs) const { return r == _rhs.r && i == _rhs.i; }
		inline bool operator!=(const complex_t<T> &_rhs) const { return r != _rhs.r || i != _rhs.i; }

		static complex_t<T> Pow(const complex_t<T> &_c) {
			return 0;
		}
};