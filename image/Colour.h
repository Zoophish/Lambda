//----	By Sam Warren 2019	----
//----	Generic colour class with 32-bit float precision for use in textures.	----

#pragma once
#include <type_traits>

class Colour {
	public:
		float r, g, b, a;

		Colour(const float _r = 0, const float _b = 0, const float _g = 0, const float _a = 1) {
			r = _r;
			b = _b;
			g = _g;
			a = _a;
		}

		inline Colour operator*(const Colour &_rhs) const {
			return Colour(r * _rhs.r, g * _rhs.g, b * _rhs.b, a * _rhs.a);
		}
		inline Colour operator*(const float _rhs) const {
			return Colour(r * _rhs, g * _rhs, b * _rhs, a * _rhs);
		}      
		inline Colour operator/(const Colour &_rhs) const {
			return Colour(r / _rhs.r, g / _rhs.g, b / _rhs.b, a / _rhs.a);
		}
		inline Colour operator/(const float _rhs) const {
			const float inv = 1.f / _rhs;
			return Colour(r * inv, g * inv, b * inv, a * inv);
		}      
		inline Colour operator+(const Colour &_rhs) const {
			return Colour(r + _rhs.r, g + _rhs.g, b + _rhs.b);
		}
		inline Colour operator-(const Colour &_rhs) const {
			return Colour(r - _rhs.r, g - _rhs.g, b - _rhs.b);
		}

		inline void operator*=(const Colour &_rhs) {
			r *= _rhs.r; g *= _rhs.g; b *= _rhs.b;
		}
		inline void operator*=(const float _rhs) {
			r *= _rhs; g *= _rhs; b *= _rhs;
		}
		inline void operator/=(const Colour &_rhs) {
			r /= _rhs.r; g /= _rhs.g; b /= _rhs.b;
		}
		inline void operator/=(const float _rhs) {
			const float inv = 1.f / _rhs;
			r *= inv;
			g *= inv;
			b *= inv;
		}
		inline void operator+=(const Colour &_rhs) {
			r += _rhs.r; g += _rhs.g; b += _rhs.b;
		}
		inline void operator-=(const Colour &_rhs) {
			r -= _rhs.r;
			g -= _rhs.g;
			b -= _rhs.b;
		}

		inline bool operator==(const Colour &_rhs) const { return r == _rhs.r && g == _rhs.g && b == _rhs.b; }
		inline bool operator!=(const Colour &_rhs) const { return r != _rhs.r || g != _rhs.g || b != _rhs.b; }

		static Colour Lerp(const Colour &_a, const Colour &_b, const float _r) {
			return _a + (_b - _a) * _r;
		}
};