//----	By Sam Warren 2019	----
//----	Generic colour class with 32-bit float precision as well as other colour formats for use in textures.	----
#pragma once

/*
	Could be struct?
*/
struct Colour {
	float r, g, b;

	Colour() {}

	Colour(const float _r, const float _b, const float _g) {
		r = _r;
		b = _b;
		g = _g;
	}

	Colour(const float _l) {
		r = _l;
		g = _l;
		b = _l;
	}

	Colour(const float *_rgb) {
		memcpy(this, _rgb, sizeof(float) * 3);
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

		RGBA32(const float *_rgba) {
			memcpy(this, _rgba, sizeof(float) * 4);
		}

		inline float operator[](const unsigned _rhs) const {
			switch (_rhs) {case 0: return r; case 1: return g; case 2: return b; case 3: return a; }
		}

		inline RGBA32 operator+(const RGBA32 &_rhs) const {
			return RGBA32(r + _rhs.r, g + _rhs.g, b + _rhs.b, a + _rhs.a);
		}

		inline RGBA32 operator-(const RGBA32 &_rhs) const {
			return RGBA32(r - _rhs.r, g - _rhs.g, b - _rhs.b, a - _rhs.a);
		}

		inline RGBA32 operator*(const float _rhs) const {
			return RGBA32(r * _rhs, g * _rhs, b * _rhs, a * _rhs);
		}
	};
}