#include "MicrofacetDistribution.h"

/*
	---------- Beckmann ----------
*/

BeckmannDistribution::BeckmannDistribution() {}

Real BeckmannDistribution::D(const Vec3 &_wh, const Vec2 &_alpha) const {
	const Real tan2Theta = Tan2Theta(_wh);
	if (std::isinf(tan2Theta)) return 0.;
	const Real cos4Theta = Cos2Theta(_wh) * Cos2Theta(_wh);
	return std::exp(-tan2Theta * (Cos2Phi(_wh) / (_alpha.x * _alpha.x) +
		Sin2Phi(_wh) / (_alpha.y * _alpha.y))) /
		(PI * _alpha.x * _alpha.y * cos4Theta);
}

Real BeckmannDistribution::Lambda(const Vec3 &_w, const Vec2 &_alpha) const {
	const Real absTanTheta = std::abs(TanTheta(_w));
	if (std::isinf(absTanTheta)) return 0.;
	const Real alpha = std::sqrt(Cos2Phi(_w) * _alpha.x * _alpha.x + Sin2Phi(_w) * _alpha.y * _alpha.y);
	const Real a = 1 / (alpha * absTanTheta);
	if (a >= 1.6f) return 0;
	return (1 - 1.259f * a + 0.396f * a * a) / (3.535f * a + 2.181f * a * a);
}

Vec3 BeckmannDistribution::Sample_wh(Sampler &_sampler, const Vec3 &_wo, const Vec2 &_alpha) const {
	const Vec2 u = _sampler.Get2D();
	const Real log = u.x == 1 ? 0 : std::log(1 - u.x);
	const Real phi = PI2 * u.y;
	const Real tan2Theta = -_alpha.x * _alpha.x * log;
	const Real cosTheta = 1 / std::sqrt(1 + tan2Theta);
	const Real sinTheta = std::sqrt(std::max((Real)0, 1 - cosTheta * cosTheta));
	Vec3 h = maths::SphericalDirection(sinTheta, cosTheta, phi);
	return !SameHemisphere(_wo, h) ? -h : h;
}

/*
	---------- Towbridge-Reitz ----------
*/

TrowbridgeReitzDistribution::TrowbridgeReitzDistribution() {}

Real TrowbridgeReitzDistribution::D(const Vec3 &_wh, const Vec2 &_alpha) const {
	const Real tan2Theta = Tan2Theta(_wh);
	if (std::isinf(tan2Theta)) return 0.;
	const Real cos4Theta = Cos2Theta(_wh) * Cos2Theta(_wh);
	const Real e = (Cos2Phi(_wh) / (_alpha.x * _alpha.x) + Sin2Phi(_wh) / (_alpha.y * _alpha.y)) * tan2Theta;
	return (Real)1 / (PI * _alpha.x * _alpha.y * cos4Theta * (1 + e) * (1 + e));
}

Real TrowbridgeReitzDistribution::Lambda(const Vec3 &_w, const Vec2 &_alpha) const {
	const Real absTanTheta = std::abs(TanTheta(_w));
	if (std::isinf(absTanTheta)) return 0.;
	const Real alpha = std::sqrt(Cos2Phi(_w) * _alpha.x * _alpha.x + Sin2Phi(_w) * _alpha.y * _alpha.y);
	const Real alpha2Tan2Theta = (alpha * absTanTheta) * (alpha * absTanTheta);
	return (-1 + std::sqrt(1.f + alpha2Tan2Theta)) / 2;
}

Vec3 TrowbridgeReitzDistribution::Sample_wh(Sampler &_sampler, const Vec3 &_wo, const Vec2 &_alpha) const {
	const Vec2 u = _sampler.Get2D();
	Real cosTheta = 0, phi = (PI2)* u.y;
	const Real tanTheta2 = _alpha.x * _alpha.x * u.x / (1 - u.x);
	cosTheta = 1 / std::sqrt(1 + tanTheta2);
	const Real sinTheta = std::sqrt(std::max((Real)0., (Real)1. - cosTheta * cosTheta));
	Vec3 wh = maths::SphericalDirection(sinTheta, cosTheta, phi);
	if (!SameHemisphere(_wo, wh)) wh = -wh;
	return wh;
}

///*
//	---------- GGX ---------
//*/
//
//GGXDistribution::GGXDistribution() {}
//
//Real GGXDistribution::D(const Vec3 &_wh, const Vec2 &_alpha) const {
//	const Real cos2Theta = Cos2Theta(_wh);
//	const Real sin2Theta = std::sqrt(1 - cos2Theta);
//	const Real tan2Theta = sin2Theta / cos2Theta;
//	const Real cos2Phi = Cos2Phi(_wh);
//	const Real sin2Phi = std::sqrt(1 - cos2Phi);
//	const Real d1 = PI * _alpha.x * _alpha.y * cos2Theta * cos2Theta;
//	const Real d2 = 1 + tan2Theta * (cos2Phi / (_alpha.x * _alpha.x) + sin2Phi / (_alpha.y * _alpha.y));
//	return 1 / (d1 * d2 * d2);
//}
//
//Real GGXDistribution::Lambda(const Vec3 &_w, const Vec2 &_alpha) const {
//	const Real xa = _alpha.x * _alpha.x * _w.x * _w.x;
//	const Real ya = _alpha.y * _alpha.y * _w.z * _w.z;
//	const Real wy2 = _w.y * _w.y;
//	const Real frac = (1 + xa + ya) / wy2;
//	return (-1 + std::sqrt(frac)) * .5;
//}
//
//Vec3 GGXDistribution::Sample_wh(Sampler &_sampler, const Vec3 &_wo, const Vec2 &_alpha) const {
//	const Vec2 u = _sampler.Get2D();
//	const Real tanPhi = _alpha.x / _alpha.y * std::tan(PI2 * u.x);
//	const Real phi = std::atan(tanPhi) + (u.x <= .25 ? 0 : (u.x < .75 ? PI : PI2));
//	const Real tan2Phi = tanPhi * tanPhi;
//	const Real sin2Phi = tan2Phi / (1 + tan2Phi);
//	const Real cos2Phi = 1 - sin2Phi;
//	const Real A = (cos2Phi / (_alpha.x * _alpha.x)) + (sin2Phi / (_alpha.y * _alpha.y));
//	const Real tan2Theta = u.y / ((1 - u.y) * A);
//	const Real sin2Theta = tan2Theta / (1 + tan2Theta);
//	const Real cos2Theta = 1 - sin2Theta;
//	return maths::SphericalDirection(std::sqrt(sin2Theta), std::sqrt(cos2Theta), phi);
//}