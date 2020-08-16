//----	By Sam Warren 2019	----
//----	Generic colour class with 32-bit float precision as well as other colour formats for use in textures.	----

//Uses the SSE operator implementations.
#pragma once

#define COLOUR_USE_SSE

#ifdef COLOUR_USE_SSE
#include <nmmintrin.h>
#endif

#include <Lambda.h>

LAMBDA_BEGIN

struct alignas(16) Colour {
	float r, g, b, a;

	Colour() {}

	Colour(const float _r, const float _g, const float _b, const float _a = 1) {
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	Colour(const float _l, const bool _alpha = false) {
		r = _l;
		g = _l;
		b = _l;
		a = _alpha ? _l : 1;
	}

	Colour(const float *_rgba, const bool _alpha = false) {
		memcpy(this, _rgba, sizeof(float) * (_alpha ? 4 : 3));
	}

	inline float operator[](const unsigned _rhs) const {
		return reinterpret_cast<float *>(const_cast<Colour *>(this))[_rhs];
	}
	inline float &operator[](const unsigned _rhs) {
		return reinterpret_cast<float *>(this)[_rhs];
	}

	#ifndef COLOUR_USE_SSE
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
			return Colour(r + _rhs.r, g + _rhs.g, b + _rhs.b, a + _rhs.a);
		}
		inline Colour operator-(const Colour &_rhs) const {
			return Colour(r - _rhs.r, g - _rhs.g, b - _rhs.b, a - _rhs.a);
		}

		inline void operator*=(const Colour &_rhs) {
			r *= _rhs.r; g *= _rhs.g; b *= _rhs.b; a *= _rhs.a;
		}
		inline void operator*=(const float _rhs) {
			r *= _rhs; g *= _rhs; b *= _rhs; a *= _rhs;
		}
		inline void operator/=(const Colour &_rhs) {
			r /= _rhs.r; g /= _rhs.g; b /= _rhs.b; a /= _rhs.a;
		}
		inline void operator/=(const float _rhs) {
			const float inv = 1.f / _rhs;
			r *= inv; g *= inv; b *= inv; a *= inv;
		}
		inline void operator+=(const Colour &_rhs) {
			r += _rhs.r; g += _rhs.g; b += _rhs.b; a += _rhs.a;
		}
		inline void operator-=(const Colour &_rhs) {
			r -= _rhs.r; g -= _rhs.g; b -= _rhs.b; a -= +_rhs.a;
		}

		inline bool operator==(const Colour &_rhs) const { return r == _rhs.r && g == _rhs.g && b == _rhs.b && a == _rhs.a; }
		inline bool operator!=(const Colour &_rhs) const { return r != _rhs.r || g != _rhs.g || b != _rhs.b || a != _rhs.a; }
	#endif

	#ifdef COLOUR_USE_SSE		
		inline Colour operator+(const Colour &_rhs) const {
			return Colour(_mm_add_ps(_mm_load_ps(reinterpret_cast<const float *>(this)), _mm_load_ps(reinterpret_cast<const float *>(&_rhs))));
		}
		inline Colour operator-(const Colour &_rhs) const {
			return Colour(_mm_sub_ps(_mm_load_ps(reinterpret_cast<const float *>(this)), _mm_load_ps(reinterpret_cast<const float *>(&_rhs))));
		}
		inline Colour operator*(const Colour &_rhs) const {
			return Colour(_mm_mul_ps(_mm_load_ps(reinterpret_cast<const float *>(this)), _mm_load_ps(reinterpret_cast<const float *>(&_rhs))));
		}
		inline Colour operator/(const Colour &_rhs) const {
			return Colour(_mm_div_ps(_mm_load_ps(reinterpret_cast<const float *>(this)), _mm_load_ps(reinterpret_cast<const float *>(&_rhs))));
		}
		inline Colour operator*(const float _rhs) const {
			return Colour(_mm_mul_ps(_mm_load_ps(reinterpret_cast<const float *>(this)), _mm_set_ps1(_rhs)));
		}
		inline Colour operator/(const float _rhs) const {
			return Colour(_mm_mul_ps(_mm_load_ps(reinterpret_cast<const float *>(this)), _mm_set_ps1(1.f / _rhs)));
		}

		inline Colour &operator+=(const Colour &_rhs) {
			_mm_store_ps(reinterpret_cast<float *>(this), _mm_add_ps(_mm_load_ps(reinterpret_cast<float *>(this)), _mm_load_ps(reinterpret_cast<const float *>(&_rhs))));
			return *this;
		}
		inline Colour &operator-=(const Colour &_rhs) {
			_mm_store_ps(reinterpret_cast<float *>(this), _mm_sub_ps(_mm_load_ps(reinterpret_cast<float *>(this)), _mm_load_ps(reinterpret_cast<const float *>(&_rhs))));
			return *this;
		}
		inline Colour &operator*=(const Colour &_rhs) {
			_mm_store_ps(reinterpret_cast<float *>(this), _mm_mul_ps(_mm_load_ps(reinterpret_cast<float *>(this)), _mm_load_ps(reinterpret_cast<const float *>(&_rhs))));
			return *this;
		}
		inline Colour &operator/=(const Colour &_rhs) {
			_mm_store_ps(reinterpret_cast<float *>(this), _mm_div_ps(_mm_load_ps(reinterpret_cast<float *>(this)), _mm_load_ps(reinterpret_cast<const float *>(&_rhs))));
			return *this;
		}
		inline Colour &operator*=(const float _rhs) {
			_mm_store_ps(reinterpret_cast<float *>(this), _mm_mul_ps(_mm_load_ps(reinterpret_cast<float *>(this)), _mm_set_ps1(_rhs)));
			return *this;
		}
		inline Colour &operator/=(const float _rhs) {
			_mm_store_ps(reinterpret_cast<float *>(this), _mm_mul_ps(_mm_load_ps(reinterpret_cast<float *>(this)), _mm_set_ps1(1.f / _rhs)));
			return *this;
		}

		inline bool operator==(const Colour &_rhs) {
			return _mm_movemask_ps(_mm_cmpeq_ps(_mm_load_ps(reinterpret_cast<float *>(this)), _mm_load_ps(reinterpret_cast<float *>(this)))) == 0xffff;
		}
		inline bool operator!=(const Colour &_rhs) {
			return _mm_movemask_ps(_mm_cmpeq_ps(_mm_load_ps(reinterpret_cast<float *>(this)), _mm_load_ps(reinterpret_cast<float *>(this)))) != 0xffff;
		}

	private:
		Colour(const __m128 &_rgba) {
			_mm_store_ps(reinterpret_cast<float *>(this), _rgba);
		}
	#endif
};

struct Colour3f {
	float r, g, b;

	Colour3f(float *_rgb) {
		memcpy(this, _rgb, sizeof(float) * 3);
	}
};

LAMBDA_END