/*----	Sam Warren 2019-2020	----
	Structure that provides access to necessary information needed for shading
	at a given point on a  surface or within a volume.
*/

#pragma once
#include <iostream>
#include <maths/maths.h>
#include <core/Ray.h>

LAMBDA_BEGIN

constexpr Real SURFACE_EPSILON = 1e-5;

class BxDF;
class Scene;
class Medium;

struct ScatterEvent {
	Vec3 wo, wi, woL, wiL;
	Real eta = 1.001;
	int sidedness = 1;	//1 = same side as normal, else = -1
	bool mediumInteraction = false;
	RayHit *hit;
	Medium *medium = nullptr;
	const Scene *scene;

	inline Vec3 ToLocal(const Vec3 &_v) const {
		return maths::WorldToLocal(_v, hit->tangent, hit->normalS, hit->bitangent);
	}
	
	inline Vec3 ToWorld(const Vec3 &_v) const {
		if(hit) return maths::LocalToWorld(_v, hit->tangent, hit->normalS, hit->bitangent);
	}

	inline void SurfaceLocalise() {
		woL = ToLocal(wo);
		wiL = ToLocal(wi);
		sidedness = (woL.y > 0) ? 1 : -1;
	}
};


//----	Surface-Space Utility Functions	----
inline bool SameHemisphere(const Vec3 &_w1, const Vec3 &_w2) {
	return _w1.y * _w2.y > 0;
}

inline Vec3 Reflect(const Vec3 &_w, const Vec3 &_n) {
	return _w - _n * 2 * maths::Dot(_n, _w);
}

inline bool Refract(const Vec3 &_w, const Vec3 &_n, const Real _eta, Vec3 *_wr) {
	const Real cosThetaI = maths::Dot(_w, _n);
	const Real sin2ThetaI = std::max((Real)0, (Real)1 - cosThetaI * cosThetaI);
	const Real sin2ThetaT = _eta * _eta * sin2ThetaI;
	if (sin2ThetaT >= 1) { //TIR
		*_wr = Vec3(-1, 1, -1) * _w;
		return false;
	}
	const Real cosThetaT = std::sqrt(1 - sin2ThetaT);
	*_wr = _w * -_eta + _n * (_eta * cosThetaI - cosThetaT);
	return true;
}


inline Real CosTheta(const Vec3 &_w) { return _w.y; }

inline Real Cos2Theta(const Vec3 &_w) { return _w.y * _w.y; }

inline Real Sin2Theta(const Vec3 &_w) {
	return std::max((Real)0, (Real)1 - Cos2Theta(_w));
}

inline Real SinTheta(const Vec3 &_w) { return std::sqrt(Sin2Theta(_w)); }

inline Real CosPhi(const Vec3 &_w) {
	const Real sinTheta = SinTheta(_w);
	return (sinTheta < 0.000001) ? 0 : maths::Clamp(_w.x / sinTheta, (Real)-1, (Real)1);
}

inline Real Cos2Phi(const Vec3 &_w) {
	return CosPhi(_w) * CosPhi(_w);
}

inline Real SinPhi(const Vec3 &_w) {
	const Real sinTheta = SinTheta(_w);
	return (sinTheta < 0.000001) ? 0 : maths::Clamp(_w.z / sinTheta, (Real)-1, (Real)1);
}

inline Real Sin2Phi(const Vec3 &_w) { return SinPhi(_w) * SinPhi(_w); }

inline Real Tan2Theta(const Vec3 &_w) {
	return Sin2Theta(_w) / Cos2Theta(_w);
}

inline Real TanTheta(const Vec3 &_w) {
	return SinTheta(_w) / CosTheta(_w);
}

LAMBDA_END