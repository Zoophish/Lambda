#include "MicrofacetDistribution.h"

/*
	---------- Beckmann ----------
*/

//BeckmannDistribution::BeckmannDistribution(const Real __alpha.x, const Real __alpha.y) {
//	_alpha.x = __alpha.x;
//	_alpha.y = __alpha.y;
//}

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

Vec3 BeckmannDistribution::Sample_wh(const Vec2 &_u, const Vec3 &_wo, const Vec2 &_alpha) const {
	if (true) {
		const Real log = _u.x == 1 ? 0 : std::log(1 - _u.x);
		const Real phi = PI2 * _u.y;
		const Real tan2Theta = -_alpha.x * _alpha.x * log;
		const Real cosTheta = 1 / std::sqrt(1 + tan2Theta);
		const Real sinTheta = std::sqrt(std::max((Real)0, 1 - cosTheta * cosTheta));
		Vec3 h = maths::SphericalDirection(sinTheta, cosTheta, phi);
		return !SameHemisphere(_wo, h) ? -h : h;
	}
}

/*
	---------- Towbridge-Reitz ----------
*/

//TrowbridgeReitzDistribution::TrowbridgeReitzDistribution(const Real __alpha.x, const Real __alpha.y) {
//	_alpha.x = __alpha.x;
//	_alpha.y = __alpha.y;
//}

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

Vec3 TrowbridgeReitzDistribution::Sample_wh(const Vec2 &_u, const Vec3 &_wo, const Vec2 &_alpha) const {
	Real cosTheta = 0, phi = (PI2)* _u.y;
	const Real tanTheta2 = _alpha.x * _alpha.x * _u.x / (1.0f - _u.x);
	cosTheta = 1 / std::sqrt(1 + tanTheta2);
	const Real sinTheta = std::sqrt(std::max((Real)0., (Real)1. - cosTheta * cosTheta));
	Vec3 wh = maths::SphericalDirection(sinTheta, cosTheta, phi);
	if (!SameHemisphere(_wo, wh)) wh = -wh;
	return wh;
}

/*
	---------- GGX ---------
*/

//GGXDistribution::GGXDistribution(const Real _sigmaX, const Real _sigmaY) {
//	_alpha.x = std::sqrt(2 * _sigmaX);
//	_alpha.y = std::sqrt(2 * _sigmaY);
//}

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
//
//}
//
//Vec3 GGXDistribution::Sample_wh(const Vec2 &_u, const Vec3 &_wo, const Vec2 &_alpha) const {
//
//}