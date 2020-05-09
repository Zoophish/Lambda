#include "GraphTexture.h"

LAMBDA_BEGIN
SG_BEGIN

namespace {
	
	constexpr double inv32 = 1. / 4294967296.;

	//Jenkins Hash
	inline uint32_t Hash(uint32_t a) {
		a = (a + 0x7ed55d16) + (a << 12);
		a = (a ^ 0xc761c23c) ^ (a >> 19);
		a = (a + 0x165667b1) + (a << 5);
		a = (a + 0xd3a2646c) ^ (a << 9);
		a = (a + 0xfd7046c5) + (a << 3);
		a = (a ^ 0xb55a4f09) ^ (a >> 16);
		return a;
	}

	inline uint32_t Hash21(uint32_t _x, uint32_t _y) {
		_x *= 1597334677u;
		_y *= 3812015801u;
		return (_x ^ _y) * 1597334677u;
	}

	inline Vec2 Hash22(Vec2 _v) {
		_v = Vec2(maths::Dot(_v, Vec2(127.1, 311.7)), maths::Dot(_v, Vec2(269.5, 183.3)));
		return maths::Fract(Vec2(std::sin(_v.x), std::sin(_v.y)) * 18.5453);
	}

	//Based on: https://www.shadertoy.com/view/4djSRW - Dave Hoskins
	inline Vec3 Hash33(Vec3 _p) {
		_p = maths::Fract(_p * Vec3(.1031, .1030, .0973));
		_p += maths::Dot(_p, Vec3(_p.y, _p.x, _p.z) + 33.33);
		return maths::Fract((Vec3(_p.x, _p.x, _p.y) + Vec3(_p.y, _p.x, _p.x)) * Vec3(_p.z, _p.y, _p.z));
	}

	inline Vec2 GetGradient2D(Real _u) {
		_u *= PI2;
		return Vec2(std::cos(_u), std::sin(_u));
	}

	inline Real Quintic(const Real _x) {
		return _x * _x * _x * (_x * (_x * 6.f - 15.f) + 10.f);
	}

	inline Real Cubic(const Real _x) {
		return _x * _x * (3 - 2 * _x);
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
		const Vec2 g00 = GetGradient2D(Hash21(p00.x, p00.y) * inv32);
		const Vec2 g10 = GetGradient2D(Hash21(p10.x, p10.y) * inv32);
		const Vec2 g11 = GetGradient2D(Hash21(p11.x, p11.y) * inv32);
		const Vec2 g01 = GetGradient2D(Hash21(p01.x, p01.y) * inv32);
		const Real dg00 = maths::Dot(_texCoords - p00, g00);
		const Real dg10 = maths::Dot(_texCoords - p10, g10);
		const Real dg11 = maths::Dot(_texCoords - p11, g11);
		const Real dg01 = maths::Dot(_texCoords - p01, g01);
		const Real vx0 = maths::Lerp(dg00, dg10, Quintic(_texCoords.x - gridX));
		const Real vx1 = maths::Lerp(dg01, dg11, Quintic(_texCoords.x - gridX));
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



	ValueNoise::ValueNoise(const Real _scale) : Node(0, 1, "Value Noise") {
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_SCALAR, &ValueNoise::GetScalar, "Scalar");
		scale = _scale;
	}

	void ValueNoise::GetScalar(const ScatterEvent &_event, void *_out) const {
		if (_event.mediumInteraction) *reinterpret_cast<Real *>(_out) = Get3D(_event.hit->point);
		else *reinterpret_cast<Real *>(_out) = Get2D(_event.hit->uvCoords);
	}

	inline Real ValueNoise::Get2D(Vec2 _texCoords) const {
		_texCoords *= scale;
		const Vec2 p00(std::floor(_texCoords.x), std::floor(_texCoords.y));
		const Vec2 p = _texCoords - p00;
		const Real v00 = Hash21(p00.x, p00.y) * inv32;
		const Real v10 = Hash21(p00.x + 1, p00.y) * inv32;
		const Real v11 = Hash21(p00.x + 1, p00.y + 1) * inv32;
		const Real v01 = Hash21(p00.x, p00.y + 1) * inv32;
		const Real vx0 = maths::Lerp(v00, v10, Cubic(p.x));
		const Real vx1 = maths::Lerp(v01, v11, Cubic(p.x));
		return maths::Lerp(vx0, vx1, Cubic(p.y));
	}

	inline Real ValueNoise::Get3D(Vec3 _point) const {
		return 0;
	}

	

	OctaveNoise::OctaveNoise(const Real _scale, const unsigned _octaves) : Node(0, 1, "Octave Noise") {
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_SCALAR, &OctaveNoise::GetScalar, "Scalar");
		scale = _scale;
		octaves = _octaves;
	}

	void OctaveNoise::GetScalar(const ScatterEvent &_event, void *_out) const {
		if (_event.mediumInteraction) *reinterpret_cast<Real *>(_out) = Get3D(_event.hit->point);
		else *reinterpret_cast<Real *>(_out) = Get2D(_event.hit->uvCoords);
	}

	Real OctaveNoise::Get2D(Vec2 _texCoords) const {
		Real out = 0, amp = .5, scl = 1;
		for (unsigned i = 0; i < octaves; ++i) {
			out += noise->Get2D(_texCoords * scale * scl) * amp;
			scl *= 2;
			amp *= .5;
		}
		return out;
	}

	Real OctaveNoise::Get3D(Vec3 _point) const {
		Real out = 0, amp = .5, scl = 1;
		for (unsigned i = 0; i < octaves; ++i) {
			out += noise->Get3D(_point * scale * scl) * amp;
			scl *= 2;
			amp *= .5;
		}
		return out;
	}



	Voronoi::Voronoi(const Real _scale) : Node(0, 1, "Voronoi") {
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_SCALAR, &Voronoi::GetScalar, "Scalar");
		scale = _scale;
	}

	void Voronoi::GetScalar(const ScatterEvent &_event, void *_out) const {
		if (_event.mediumInteraction) *reinterpret_cast<Real *>(_out) = Get3D(_event.hit->point);
		else *reinterpret_cast<Real *>(_out) = Get2D(_event.hit->uvCoords);
	}

	Real Voronoi::Get2D(Vec2 _texCoords) const {
		_texCoords *= scale;
		const Vec2 p00(std::floor(_texCoords.x), std::floor(_texCoords.y));
		Real minDist2 = INFINITY;
		for (int y = -1; y < 2; ++y) {
			for (int x = -1; x < 2; ++x) {
				const Vec2 pOff = p00 + Vec2(x, y);
				const Vec2 rp = pOff + Hash22(pOff) - _texCoords;
				const Real dist2 = maths::Dot(rp, rp);
				if (dist2 < minDist2) minDist2 = dist2;
			}
		}
		return std::sqrt(minDist2);
	}

	Real Voronoi::Get3D(Vec3 _point) const {
		_point *= scale;
		const Vec3 p00(std::floor(_point.x), std::floor(_point.y), std::floor(_point.z));
		Real minDist2 = INFINITY;
		for (int z = -1; z < 2; ++z) {
			for (int y = -1; y < 2; ++y) {
				for (int x = -1; x < 2; ++x) {
					const Vec3 pOff = p00 + Vec3(x, y, z);
					const Vec3 rp = pOff + Hash33(pOff) - _point;
					const Real dist2 = maths::Dot(rp, rp);
					if (dist2 < minDist2) minDist2 = dist2;
				}
			}
		}
		return std::sqrt(minDist2);
	}

}

SG_END
LAMBDA_END