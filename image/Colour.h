//----	By Sam Warren 2019	----
//----	Generic colour class with 32-bit float precision as well as other colour formats for use in textures.	----

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

	inline float operator[](const unsigned _rhs) const {
		return reinterpret_cast<float *>(const_cast<Colour*>(this))[_rhs];
	}
	inline float &operator[](const unsigned _rhs) {
		return reinterpret_cast<float *>(this)[_rhs];
	}
};