/*
Structure that provides access to necessary information needed for shading.
*/

//TO DO: Rename to SurfaceEvent?
#pragma once
#include <maths/maths.h>
#include <core/Ray.h>
class BxDF;
class Scene;


//Align?
struct SurfaceScatterEvent {
	Vec3 wo, wi, woL, wiL;
	Real pdf, eta = 1.;
	RayHit *hit;
	bool isect;
	const Scene *scene;

	inline Vec3 ToLocal(const Vec3 &_v) const {
		if(hit) return maths::ToSpace(_v, hit->tangent, hit->normalS, hit->bitangent);
	}
	
	inline Vec3 ToWorld(const Vec3 &_v) const {
		if(hit) return maths::FromSpace(_v, hit->tangent, hit->normalS, hit->bitangent);
	}

	inline void Localise() {
		woL = ToLocal(wo);
		wiL = ToLocal(wi);
	}
};


//----	Surface-Space Utility Functions	----
inline Real CosTheta(const Vec3 &_w) { return _w.y; }

inline Real Cos2Theta(const Vec3 &_w) { return _w.y * _w.y; }

inline Real Sin2Theta(const Vec3 &_w) { return std::max((Real)0, 1 - _w.y * _w.y); }

inline Real SinTheta(const Vec3 &_w) { return std::sqrt(Sin2Theta(_w)); }

inline Real CosPhi(const Vec3 &_w) {
	const Real sinTheta = SinTheta(_w);
	return (sinTheta == 0) ? 0 : maths::Clamp(_w.x / sinTheta, (Real)-1, (Real)1);
}

inline Real Cos2Phi(const Vec3 &_w) {
	return CosPhi(_w) * CosPhi(_w);
}

inline Real SinPhi(const Vec3 &_w) {
	const Real sinTheta = SinTheta(_w);
	return (sinTheta == 0) ? 0 : maths::Clamp(_w.y / sinTheta, (Real)-1, (Real)1);
}

inline Real Sin2Phi(const Vec3 &_w) {
	return SinPhi(_w) * SinPhi(_w);
}

inline Real Tan2Theta(const Vec3 &_w) {
	return Sin2Theta(_w) / Cos2Theta(_w);
}

inline Real TanTheta(const Vec3 &_w) {
	return SinTheta(_w) / CosTheta(_w);
}