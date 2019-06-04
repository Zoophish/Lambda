//----	By Sam Warren 2019	----
//----	Single include header for mathematics library. Typdefs of general numerical types with overall precision control can be put here.	----

#pragma once
#include "real_t.h"
#include "vec3.h"
#include "vec2.h"

#define LAMBDA_PRECISION_MODE float
#define LAMBDA_MATHS_COMMON_TYPES

typedef real_t<LAMBDA_PRECISION_MODE> Real;
typedef vec3<LAMBDA_PRECISION_MODE> Vec3;
typedef vec2<LAMBDA_PRECISION_MODE> Vec2;

#ifdef LAMBDA_MATHS_COMMON_TYPES
typedef real_t<float> realf;
typedef real_t<double> reald;
typedef real_t<int> reali;
typedef real_t<unsigned> realu;

typedef vec3<float> vec3f;
typedef vec3<double> vec3d;
typedef vec3<int> vec3i;
typedef vec3<unsigned> vec3u;

typedef vec2<float> vec2f;
typedef vec2<double> vec2d;
typedef vec2<int> vec2i;
typedef vec2<unsigned> vec2u;
#endif