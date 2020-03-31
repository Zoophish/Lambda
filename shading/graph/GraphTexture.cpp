#include "GraphTexture.h"

LAMBDA_BEGIN
SG_BEGIN

namespace {

	inline uint32_t Hash(uint32_t a) {
		a = (a + 0x7ed55d16) + (a << 12);
		a = (a ^ 0xc761c23c) ^ (a >> 19);
		a = (a + 0x165667b1) + (a << 5);
		a = (a + 0xd3a2646c) ^ (a << 9);
		a = (a + 0xfd7046c5) + (a << 3);
		a = (a ^ 0xb55a4f09) ^ (a >> 16);
		return a;
	}

	inline uint32_t Hash2D(uint32_t _x, uint32_t _y) {
		_x *= 1597334677u;
		_y *= 3812015801u;
		return (_x ^ _y) * 1597334677u;
	}

	inline uint32_t Hash3D(uint32_t _x, uint32_t _y, uint32_t _z) {
		return 0;
	}

	inline Vec2 GetGradient(Real _u) {
		_u *= PI2;
		return Vec2(std::cos(_u), std::sin(_u));
	}

	inline Real Quintic(float x) {
		return x * x * x * (x * (x * 6.f - 15.f) + 10.f);
	}

}

namespace Textures {

	PerlinNoise::PerlinNoise(const Real _scale) : Node(0, 1, "Perlin Noise") {
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_SCALAR, &PerlinNoise::GetScalar, "Amplitude");
		scale = _scale;
	}

	void PerlinNoise::GetScalar(const ScatterEvent &_event, void *_out) const {
		if (_event.mediumInteraction) *reinterpret_cast<Real *>(_out) = Get3D(_event.hit->point);
		else *reinterpret_cast<Real *>(_out) = Get2D(_event.hit->uvCoords);
	}

	Real PerlinNoise::Get2D(Vec2 _texCoords) const {
		_texCoords *= scale;
		const Real gridX = std::floor(_texCoords.x);
		const Real gridY = std::floor(_texCoords.y);
		const Vec2 p00(gridX, gridY);
		const Vec2 p10(gridX + 1, gridY);
		const Vec2 p11(gridX + 1, gridY + 1);
		const Vec2 p01(gridX, gridY + 1);
		const Vec2 g00 = GetGradient(Hash2D(p00.x, p00.y));
		const Vec2 g10 = GetGradient(Hash2D(p10.x, p10.y));
		const Vec2 g11 = GetGradient(Hash2D(p11.x, p11.y));
		const Vec2 g01 = GetGradient(Hash2D(p01.x, p01.y));
		const Real dg00 = maths::Dot(_texCoords - p00, g00);
		const Real dg10 = maths::Dot(_texCoords - p10, g10);
		const Real dg11 = maths::Dot(_texCoords - p11, g11);
		const Real dg01 = maths::Dot(_texCoords - p01, g01);
		const Real vx0 = maths::Lerp(dg00, dg10, _texCoords.x - gridX);
		const Real vx1 = maths::Lerp(dg01, dg11, _texCoords.x - gridX);
		const Real v = maths::Lerp(vx0, vx1, Quintic(_texCoords.y - gridY));
		return v + .5;
	}

	Real PerlinNoise::Get3D(Vec3 _point) const {
		_point *= scale;
		return 0;
	}



	Checker::Checker(const Real _scale) : Node(0, 1, "Checker") {
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_SCALAR, &Checker::GetScalar, "Scalar");
		scale = _scale;
	}

	void Checker::GetScalar(const ScatterEvent &_event, void *_out) const {
		if (_event.mediumInteraction) *reinterpret_cast<Real *>(_out) = Get3D(_event.hit->point);
		else *reinterpret_cast<Real *>(_out) = Get2D(_event.hit->uvCoords);
	}

	inline Real Checker::Get2D(Vec2 _texCoords) const {
		_texCoords = maths::Fract(_texCoords * scale);
		return (_texCoords.x > .5f) == (_texCoords.y > .5f) ? 0 : 1;
	}

	inline Real Checker::Get3D(Vec3 _point) const {
		_point = maths::Fract(_point * scale);
		return (_point.x > .5) == (_point.y > .5) == (_point.z > .5) ? 0 : 1;
	}

}


SG_END
LAMBDA_END