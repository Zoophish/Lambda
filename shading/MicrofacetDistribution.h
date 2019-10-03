#pragma once
#include <maths/maths.h>
#include "surface/BxDF.h"

class MicrofacetDistribution {
	public:
		virtual Real D(const Vec3 &_w) const = 0;

		virtual Real Lambda(const Vec3 &_w) const = 0;

		virtual Vec3 Sample_wh(const Vec2 &_u, const Vec3 &_wo) const = 0;

		Real Pdf(const Vec3 &_wh) const { return D(_wh) * std::abs(_wh.y); }
		
		Real G1(const Vec3 &_w) const { return (Real)1 / ((Real)1 + Lambda(_w)); }

		Real G(const Vec3 &_wo, const Vec3 &_wi) const { return (Real)1 / ((Real)1 + Lambda(_wo) + Lambda(_wi)); }

		static inline Real RoughnessToAlpha(Real _roughness) {
			_roughness = std::max(_roughness, (Real)1e-3);
			Real x = std::log(_roughness);
			return 1.62142f + 0.819955f * x + 0.1734f * x * x +
				0.0171201f * x * x * x + 0.000640711f * x * x * x * x;
		}
};

class BeckmannDistribution : public MicrofacetDistribution {
	public:
		BeckmannDistribution(const Real _alphaX, const Real _alphaY) {
			alphaX = _alphaX;
			alphaY = _alphaY;
		}

		inline void SetSigmas(const Real _sigmaX, const Real _sigmaY) {
			alphaX = std::sqrt(2 * _sigmaX);
			alphaY = std::sqrt(2 * _sigmaY);
		}

		Real D(const Vec3 &_wh) const override {
			Real tan2Theta = Tan2Theta(_wh);
			if (std::isinf(tan2Theta)) return 0.;
			Real cos4Theta = Cos2Theta(_wh) * Cos2Theta(_wh);
			return std::exp(-tan2Theta * (Cos2Phi(_wh) / (alphaX * alphaX) +
				Sin2Phi(_wh) / (alphaY * alphaY))) /
				(PI * alphaX * alphaY * cos4Theta);
		}

		Real Lambda(const Vec3 &_w) const override {
			const Real absTanTheta = std::abs(TanTheta(_w));
			if (std::isinf(absTanTheta)) return 0.;
			const Real alpha = std::sqrt(Cos2Phi(_w) * alphaX * alphaX + Sin2Phi(_w) * alphaY * alphaY);
			const Real a = 1 / (alpha * absTanTheta);
			if (a >= 1.6f) return 0;
			return (1 - 1.259f * a + 0.396f * a * a) / (3.535f * a + 2.181f * a * a);
		}

		Vec3 Sample_wh(const Vec2 &_u, const Vec3 &_wo) const override {
			if (true) {
				const Real log = _u.x == 1 ? 0 : std::log(1 - _u.x);
				const Real phi = PI2 * _u.y;
				const Real tan2Theta = -alphaX * alphaX * log;
				const Real cosTheta = 1 / std::sqrt(1 + tan2Theta);
				const Real sinTheta = std::sqrt(std::max((Real)0, 1 - cosTheta * cosTheta));
				Vec3 h =  maths::SphericalDirection(sinTheta, cosTheta, phi);
				return !SameHemisphere(_wo, h) ? -h : h;
			}
		}

	protected:
		Real alphaX, alphaY;
};

class TrowbridgeReitzDistribution : public MicrofacetDistribution {
	public:
		TrowbridgeReitzDistribution(const Real _alphaX, const Real _alphaY) {
			alphaX = _alphaX;
			alphaY = _alphaY;
		}

		Real D(const Vec3 &_wh) const override {
			Real tan2Theta = Tan2Theta(_wh);
			if (std::isinf(tan2Theta)) return 0.;
			const Real cos4Theta = Cos2Theta(_wh) * Cos2Theta(_wh);
			Real e =
				(Cos2Phi(_wh) / (alphaX * alphaX) + Sin2Phi(_wh) / (alphaY * alphaY)) *
				tan2Theta;
			return (Real)1 / (PI * alphaX * alphaY * cos4Theta * (1 + e) * (1 + e));
		}

		Real Lambda(const Vec3 &_w) const override {
			Real absTanTheta = std::abs(TanTheta(_w));
			if (std::isinf(absTanTheta)) return 0.;
			Real alpha =
				std::sqrt(Cos2Phi(_w) * alphaX * alphaX + Sin2Phi(_w) * alphaY * alphaY);
			Real alpha2Tan2Theta = (alpha * absTanTheta) * (alpha * absTanTheta);
			return (-1 + std::sqrt(1.f + alpha2Tan2Theta)) / 2;
		}

		Vec3 Sample_wh(const Vec2 &_u, const Vec3 &_wo) const override {
			Real cosTheta = 0, phi = (PI2) * _u.y;
			Real tanTheta2 = alphaX * alphaX * _u.x / (1.0f - _u.x);
			cosTheta = 1 / std::sqrt(1 + tanTheta2);
			Real sinTheta =
				std::sqrt(std::max((Real)0., (Real)1. - cosTheta * cosTheta));
			Vec3 wh = maths::SphericalDirection(sinTheta, cosTheta, phi);
			if (!SameHemisphere(_wo, wh)) wh = -wh;
			return wh;
		}

	protected:
		Real alphaX, alphaY;
};

class GGXDistribution : public MicrofacetDistribution {
	public:
		GGXDistribution(const Real _sigmaX, const Real _sigmaY) {
			alphaX = std::sqrt(2 * _sigmaX);
			alphaY = std::sqrt(2 * _sigmaY);
		}

		Real D(const Vec3 &_wh) const override {
			const Real cos2Theta = Cos2Theta(_wh);
			const Real sin2Theta = std::sqrt(1 - cos2Theta);
			const Real tan2Theta = sin2Theta / cos2Theta;
			const Real cos2Phi = Cos2Phi(_wh);
			const Real sin2Phi = std::sqrt(1 - cos2Phi);
			const Real d1 = PI * alphaX * alphaY * cos2Theta * cos2Theta;
			const Real d2 = 1 + tan2Theta * (cos2Phi / (alphaX*alphaX) + sin2Phi / (alphaY*alphaY));
			return 1 / (d1 * d2 * d2);
		}

		Real Lambda(const Vec3 &_w) const override {

		}

		Vec3 Sample_wh(const Vec2 &_u, const Vec3 &_wo) const override {

		}

	protected:
		Real alphaX, alphaY;
};