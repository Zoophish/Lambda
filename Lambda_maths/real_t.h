//----	By Sam Warren 2019	----
//----	Template class for real numbers of different precisions. Changing large parts of program's precision is easier.	----

#pragma once
#include <type_traits>

template<
	typename T, 
	typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
>
class real_t {
	protected:
		T value;

	public:
		real_t(const T _value = 0) {
			value = _value;
		}

		inline operator T() const { return value; }

		inline void operator=(const T _rhs) { value = _rhs; }

		inline real_t operator+(const T _rhs) const { return value + _rhs; }
		inline real_t operator-(const T _rhs) const { return value - _rhs; }
		inline real_t operator*(const T _rhs) const { return value * _rhs; }
		inline real_t operator/(const T _rhs) const { return value / _rhs; }

		inline void operator+=(const T _rhs) { value +=	_rhs; }
		inline void operator-=(const T _rhs) { value -=	_rhs; }
		inline void operator*=(const T _rhs) { value *=	_rhs; }
		inline void operator/=(const T _rhs) { value /=	_rhs; }

		inline T operator++() { return ++value; }
		inline T operator--() { return --value; }
		inline T operator++(int) { return value++; }
		inline T operator--(int) { return value--; }

		inline bool operator==(const T _rhs) const { return value == _rhs; }
		inline bool operator!=(const T _rhs) const { return value != _rhs; }
		inline bool operator>(const T _rhs) const { return value > _rhs; }
		inline bool operator<(const T _rhs) const { return value < _rhs; }
		inline bool operator<=(const T _rhs) const { return value <= _rhs; }
		inline bool operator>=(const T _rhs) const { return value >= _rhs; }
};