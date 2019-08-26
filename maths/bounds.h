#pragma once
#include <type_traits>
#include "vec3.h"

template<
	typename T,
	typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
>
struct bounds {
	vec3<T> min, max;

	bounds() {}

	bounds(const vec3<T> &_min, const vec3<T> &_max) {
		min = _min;
		max = _max;
	}
};

template<>
struct alignas(16) bounds<float> {
	vec3<float> min, max;

	bounds() {}

	bounds(const vec3<float> &_min, const vec3<float> &_max) {
		min = _min;
		max = _max;
	}
};