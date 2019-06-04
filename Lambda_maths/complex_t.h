#pragma once
#include <type_traits>

template<
	typename T,
	typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
>
class complex_t {
	public:
		T a, b;

		complex_t(const T _a = 0, const T _b = 0) {
			a = _a;
			b = _b;
		}


};