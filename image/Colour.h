//----	By Sam Warren 2019	----
//----	Generic colour class with 32-bit float precision as well as other colour formats for use in textures.	----
#pragma once

class Colour {
	public:
		float r, g, b;

		Colour(const float _r = 0, const float _b = 0, const float _g = 0) {
			r = _r;
			b = _b;
			g = _g;
		}

		Colour(float *_rgb) {
			r = _rgb[0];
			g = _rgb[1];
			b = _rgb[2];
		}

		inline Colour operator*(const Colour &_rhs) const {
			return Colour(r * _rhs.r, g * _rhs.g, b * _rhs.b);
		}
		inline Colour operator*(const float _rhs) const {
			return Colour(r * _rhs, g * _rhs, b * _rhs);
		}      
		inline Colour operator/(const Colour &_rhs) const {
			return Colour(r / _rhs.r, g / _rhs.g, b / _rhs.b);
		}
		inline Colour operator/(const float _rhs) const {
			const float inv = 1.f / _rhs;
			return Colour(r * inv, g * inv, b * inv);
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

namespace ColourFormat {

	typedef Colour RGB32;

	struct RGBA32 {
		float r, g, b, a;

		RGBA32(const float _r = 0, const float _g = 0, const float _b = 0, const float _a = 0) {
			r = _r;
			g = _g;
			b = _b;
			a = _a;
		}
	};

	struct R32 {
		float r;

		R32() {}

		R32(const float _r) {
			r = _r;
		}
	};
}