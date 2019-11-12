#include "MicrofacetDistribution.h"

/*
	---------- Beckmann ----------
*/


BeckmannDistribution::BeckmannDistribution(const Real _alphaX, const Real _alphaY) {
	alphaX = _alphaX;
	alphaY = _alphaY;
}

Real BeckmannDistribution::D(const Vec3 &_wh) const {
	const Real tan2Theta = Tan2Theta(_wh);
	if (std::isinf(tan2Theta)) return 0.;
	const Real cos4Theta = Cos2Theta(_wh) * Cos2Theta(_wh);
	return std::exp(-tan2Theta * (Cos2Phi(_wh) / (alphaX * alphaX) +
		Sin2Phi(_wh) / (alphaY * alphaY))) /
		(PI * alphaX * alphaY * cos4Theta);
}

Real BeckmannDistribution::Lambda(const Vec3 &_w) const {
	const Real absTanTheta = std::abs(TanTheta(_w));
	if (std::isinf(absTanTheta)) return 0.;
	const Real alpha = std::sqrt(Cos2Phi(_w) * alphaX * alphaX + Sin2Phi(_w) * alphaY * alphaY);
	const Real a = 1 / (alpha * absTanTheta);
	if (a >= 1.6f) return 0;
	return (1 - 1.259f * a + 0.396f * a * a) / (3.535f * a + 2.181f * a * a);
}

Vec3 BeckmannDistribution::Sample_wh(const Vec2 &_u, const Vec3 &_wo) const {
	if (true) {
		const Real log = _u.x == 1 ? 0 : std::log(1 - _u.x);
		const Real phi = PI2 * _u.y;
		const Real tan2Theta = -alphaX * alphaX * log;
		const Real cosTheta = 1 / std::sqrt(1 + tan2Theta);
		const Real sinTheta = std::sqrt(std::max((Real)0, 1 - cosTheta * cosTheta));
		Vec3 h = maths::SphericalDirection(sinTheta, cosTheta, phi);
		return !SameHemisphere(_wo, h) ? -h : h;
	}
}

/*
	---------- Towbridge-Reitz ----------
*/

TrowbridgeReitzDistribution::TrowbridgeReitzDistribution(const Real _alphaX, const Real _alphaY) {
	alphaX = _alphaX;
	alphaY = _alphaY;
}

Real TrowbridgeReitzDistribution::D(const Vec3 &_wh) const {
	const Real tan2Theta = Tan2Theta(_wh);
	if (std::isinf(tan2Theta)) return 0.;
	const Real cos4Theta = Cos2Theta(_wh) * Cos2Theta(_wh);
	const Real e = (Cos2Phi(_wh) / (alphaX * alphaX) + Sin2Phi(_wh) / (alphaY * alphaY)) * tan2Theta;
	return (Real)1 / (PI * alphaX * alphaY * cos4Theta * (1 + e) * (1 + e));
}

Real TrowbridgeReitzDistribution::Lambda(const Vec3 &_w) const {
	const Real absTanTheta = std::abs(TanTheta(_w));
	if (std::isinf(absTanTheta)) return 0.;
	const Real alpha = std::sqrt(Cos2Phi(_w) * alphaX * alphaX + Sin2Phi(_w) * alphaY * alphaY);
	const Real alpha2Tan2Theta = (alpha * absTanTheta) * (alpha * absTanTheta);
	return (-1 + std::sqrt(1.f + alpha2Tan2Theta)) / 2;
}

Vec3 TrowbridgeReitzDistribution::Sample_wh(const Vec2 &_u, const Vec3 &_wo) const {
	Real cosTheta = 0, phi = (PI2)* _u.y;
	const Real tanTheta2 = alphaX * alphaX * _u.x / (1.0f - _u.x);
	cosTheta = 1 / std::sqrt(1 + tanTheta2);
	const Real sinTheta = std::sqrt(std::max((Real)0., (Real)1. - cosTheta * cosTheta));
	Vec3 wh = maths::SphericalDirection(sinTheta, cosTheta, phi);
	if (!SameHemisphere(_wo, wh)) wh = -wh;
	return wh;
}