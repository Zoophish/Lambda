//----	By Sam Warren 2019	----
//----	Single include header for basic mathematics library needed in Lambda.	----

//----	Define the general numerical precision here:
#define LAMBDA_MATHS_PRECISION_MODE float

//----	Optional basic typdefs for mathematical types:
//#define LAMBDA_MATHS_COMMON_TYPES



//----	Don't make changes below here.
#pragma once
#include <cmath>
#include <algorithm>
#include "real_t.h"
#include "complex_t.h"
#include "vec3.h"
#include "vec2.h"

typedef LAMBDA_MATHS_PRECISION_MODE Real;
typedef complex_t<Real> Complex;
typedef vec3<Real> Vec3;
typedef vec2<Real> Vec2;

#define PI 3.14159265358979323846264338
#define INV_PI 1. / PI

namespace maths {

	template<class T>
	inline T Lerp(const T &_a, const T &_b, const Real _r) {
		return _a + (_b - _a) * _r;
	}

	template<class T>
	inline T Clamp(const T _value, const T _min, const T _max) {
		return std::max(std::min(_value, _max), _min);
	}

}

#ifdef LAMBDA_MATHS_COMMON_TYPES
typedef real_t<float> realf;
typedef real_t<double> reald;
typedef real_t<int> reali;
typedef real_t<unsigned> realu;

typedef complex_t<float> complexf;
typedef complex_t<double> complexd;
typedef complex_t<int> complexi;
typedef complex_t<unsigned> complexu;

typedef vec3<float> vec3f;
typedef vec3<double> vec3d;
typedef vec3<int> vec3i;
typedef vec3<unsigned> vec3u;

typedef vec2<float> vec2f;
typedef vec2<double> vec2d;
typedef vec2<int> vec2i;
typedef vec2<unsigned> vec2u;
#endif