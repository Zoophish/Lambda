/*----	By Sam Warren 2019	----
Template class for 3-dimensional vectors of any numerical type. Includes basic operators and utilities.
Supports SSE instructions for vec3<float> which offers a significant performance improvement.
*/
#define LAMBDA_VEC3_USE_SSE

#pragma once
#include <type_traits>
#include <nmmintrin.h>

template <
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

		inline vec3<T> operator+(const vec3<T> &_rhs) const { return vec3<T>(x + _rhs.x, y + _rhs.y, z + _rhs.z); }
		inline vec3<T> operator-(const vec3<T> &_rhs) const { return vec3<T>(x - _rhs.x, y - _rhs.y, z - _rhs.z); }
		inline vec3<T> operator*(const vec3<T> &_rhs) const { return vec3<T>(x * _rhs.x, y * _rhs.y, z * _rhs.z); }
		inline vec3<T> operator*(const T _rhs) const { return vec3<T>(x * _rhs, y * _rhs, z * _rhs); }
		inline vec3<T> operator/(const vec3<T> &_rhs) const { return vec3<T>(x / _rhs.x, y / _rhs.y, z / _rhs.z); }
		inline vec3<T> operator/(const T _rhs) const { const T inv = (T)1 / _rhs; return vec3<T>(x * inv, y * inv, z * inv); }
		inline vec3<T> operator-() const { return *this * -1; }

		inline vec3<T>& operator+=(const vec3<T> &_rhs) {
			x += _rhs.x;
			y += _rhs.y;
			z += _rhs.z;
			return *this;
		}
		inline vec3<T>& operator-=(const vec3<T> &_rhs) {
			x -= _rhs.x;
			y -= _rhs.y;
			z -= _rhs.z;
			return *this;
		}
		inline vec3<T>& operator*=(const vec3<T> &_rhs) {
			x *= _rhs.x;
			y *= _rhs.y;
			z *= _rhs.z;
			return *this;
		}
		inline vec3<T>& operator*=(const T _rhs) {
			x *= _rhs;
			y *= _rhs;
			z *= _rhs;
			return *this;
		}
		inline vec3<T>& operator/=(const vec3<T> &_rhs) {
			x /= _rhs.x;
			y /= _rhs.y;
			z /= _rhs.z;
			return *this;
		}
		inline vec3<T>& operator/=(const T _rhs) {
			const T inv = (T)1 / _rhs;
			x *= inv;
			y *= inv;
			z *= inv;
			return *this;
		}
		
		inline bool operator==(const vec3<T> &_rhs) const { return x == _rhs.x && y == _rhs.y && z == _rhs.z; }
		inline bool operator!=(const vec3<T> &_rhs) const { return x != _rhs.x || y != _rhs.y || z != _rhs.z; }

		inline T Magnitude() const { return std::sqrt(x * x + y * y + z * z); }

		inline vec3<T> Normalised() const {
			const T inv = (T)1 / Magnitude();
			return *this * inv;
		}
};

/*
----	SIMD SSE Vec3 Implementation	----
Since float will almost always be used, this introduces a large performance boost for basic operators.
Since SSE uses 128-bit registers, there is a redundant fourth float 'a' to indicate the alignment of 16 bytes.
*/
#ifdef LAMBDA_VEC3_USE_SSE
template<>
class alignas(16) vec3<float> {
	public:
		float x, y, z, a;

		vec3(const float _x = 0, const float _y = 0, const float _z = 0) {
			x = _x;
			y = _y;
			z = _z;
			a = 0;
		}

		inline vec3<float> operator+(const vec3<float> &_rhs) const {
			return vec3<float>(_mm_add_ps(_mm_load_ps(reinterpret_cast<const float*>(this)), _mm_load_ps(reinterpret_cast<const float*>(&_rhs))));
		}
		inline vec3<float> operator-(const vec3<float> &_rhs) const {
			return vec3<float>(_mm_sub_ps(_mm_load_ps(reinterpret_cast<const float*>(this)), _mm_load_ps(reinterpret_cast<const float*>(&_rhs))));
		}
		inline vec3<float> operator*(const vec3<float> &_rhs) const {
			return vec3<float>(_mm_mul_ps(_mm_load_ps(reinterpret_cast<const float*>(this)), _mm_load_ps(reinterpret_cast<const float*>(&_rhs))));
		}
		inline vec3<float> operator/(const vec3<float> &_rhs) const {
			return vec3<float>(_mm_div_ps(_mm_load_ps(reinterpret_cast<const float*>(this)), _mm_load_ps(reinterpret_cast<const float*>(&_rhs))));
		}
		inline vec3<float> operator*(const float _rhs) const {
			return vec3<float>(_mm_mul_ps(_mm_load_ps(reinterpret_cast<const float*>(this)), _mm_set_ps1(_rhs)));
		}
		inline vec3<float> operator/(const float _rhs) const {
			return vec3<float>(_mm_mul_ps(_mm_load_ps(reinterpret_cast<const float*>(this)), _mm_set_ps1(1.f / _rhs)));
		}
		inline vec3<float> operator-() const { return *this * -1; }

		inline vec3<float>& operator+=(const vec3<float> &_rhs) {
			_mm_store_ps(reinterpret_cast<float*>(this), _mm_add_ps(_mm_load_ps(reinterpret_cast<float*>(this)), _mm_load_ps(reinterpret_cast<const float*>(&_rhs))));
			return *this;
		}
		inline vec3<float>& operator-=(const vec3<float> &_rhs) {
			_mm_store_ps(reinterpret_cast<float*>(this), _mm_sub_ps(_mm_load_ps(reinterpret_cast<float*>(this)), _mm_load_ps(reinterpret_cast<const float*>(&_rhs))));
			return *this;
		}
		inline vec3<float>& operator*=(const vec3<float> &_rhs) {
			_mm_store_ps(reinterpret_cast<float*>(this), _mm_mul_ps(_mm_load_ps(reinterpret_cast<float*>(this)), _mm_load_ps(reinterpret_cast<const float*>(&_rhs))));
			return *this;
		}
		inline vec3<float>& operator/=(const vec3<float> &_rhs) {
			_mm_store_ps(reinterpret_cast<float*>(this), _mm_div_ps(_mm_load_ps(reinterpret_cast<float*>(this)), _mm_load_ps(reinterpret_cast<const float*>(&_rhs))));
			return *this;
		}
		inline vec3<float>& operator*=(const float _rhs) {
			_mm_store_ps(reinterpret_cast<float*>(this), _mm_mul_ps(_mm_load_ps(reinterpret_cast<float*>(this)), _mm_set_ps1(_rhs)));
			return *this;
		}
		inline vec3<float>& operator/=(const float _rhs) {
			_mm_store_ps(reinterpret_cast<float*>(this), _mm_mul_ps(_mm_load_ps(reinterpret_cast<float*>(this)), _mm_set_ps1(1.f / _rhs)));
			return *this;
		}

		inline bool operator==(const vec3<float> &_rhs) const { return x == _rhs.x && y == _rhs.y && z == _rhs.z; }
		inline bool operator!=(const vec3<float> &_rhs) const { return x != _rhs.x || y != _rhs.y || z != _rhs.z; }

		inline float Magnitude() const {
			const vec3<float> sq = *this * *this;
			return std::sqrt(sq.x + sq.y + sq.z);
		}

		inline vec3<float> Normalised() const {
			const float inv = 1.f / Magnitude();
			return *this * inv;
		}

		protected:
			vec3(const __m128 &_xyza) {
				_mm_store_ps(reinterpret_cast<float*>(this), _xyza);
			}
};
#endif

namespace maths {

	template<class T>
	inline T Dot(const vec3<T> &_a, const vec3<T> &_b) {
		return _a.x*_b.x + _a.y*_b.y + _a.z*_b.z;
	}

	template<class T>
	inline vec3<T> Cross(const vec3<T> &_a, const vec3<T> &_b) {
		return vec3<T>(_a.y*_b.z - _a.z*_b.y, _a.z*_b.x - _a.x*_b.z, _a.x*_b.y - _a.y*_b.x);
	}

	template<class T>
	inline T DistSq(const vec3<T> &_a, const vec3<T> &_b) {
		const vec3<T> diff = _b - _a;
		return Dot(diff, diff);
	}

	template<class T>
	inline vec3<T> SphericalDirection(const T _sinTheta, const T _cosTheta, const T _phi) {
		return vec3<T>(_sinTheta * std::cos(_phi), _cosTheta, _sinTheta * std::sin(_phi));
	}

	template<class T>
	inline T SphericalTheta(const vec3<T> &_v) {
		return std::acos(std::max(std::min(_v.y, (T)1.), (T)-1.));
	}

	template<class T>
	inline T SphericalPhi(const vec3<T> &_v) {
		const T p = std::atan2(_v.z, _v.x);
		return (p < 0.) ? (p + 6.2831853) : p;
	}

	#ifdef LAMBDA_VEC3_USE_SSE
	template<>
	inline float Dot(const vec3<float> &_a, const vec3<float> &_b) {
		const vec3<float> sq = _a * _b;
		return sq.x + sq.y + sq.z;
	}
	#endif
}