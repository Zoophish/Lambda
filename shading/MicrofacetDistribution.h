#pragma once
#include <maths/maths.h>
#include "BxDF.h"

class MicrofacetDistribution {
	public:
		virtual Real D(const Vec3 &_w) const = 0;

		virtual Real Lambda(const Vec3 &_w) const = 0;

		Real G1(const Vec3 &_w) const { return 1 / (1 + Lambda(_w)); }

		Real G(const Vec3 &_wo, const Vec3 &_wi) const { return 1 / (1 + Lambda(_wo) + Lambda(_wi)); }
};

class BeckmannDistribution : public MicrofacetDistribution {
	public:
		BeckmannDistribution(const Real _sigmaX, const Real _sigmaY) {
			SetSigmas(_sigmaX, _sigmaY);
		}

		inline void SetSigmas(const Real _sigmaX, const Real _sigmaY) {
			alphaX = std::sqrt(2 * _sigmaX);
			alphaY = std::sqrt(2 * _sigmaY);
		}

		Real D(const Vec3 &_w) const override {
			const Real tan2Theta = Tan2Theta(_w);
			if (std::isinf(tan2Theta)) return 0.;
			const Real cos4Theta = Cos2Theta(_w) * Cos2Theta(_w);
			return std::exp(-tan2Theta * (Cos2Phi(_w) / (alphaX * alphaX) + Sin2Phi(_w) / (alphaY * alphaY))) / (PI * alphaX * alphaY * cos4Theta);
		}

		Real Lambda(const Vec3 &_w) const override {
			const Real absTanTheta = std::abs(TanTheta(_w));
			if (std::isinf(absTanTheta)) return 0.;
				const Real alpha = std::sqrt(Cos2Phi(_w) * alphaX * alphaX + Sin2Phi(_w) * alphaY * alphaY);
			const Real a = 1 / (alpha * absTanTheta);
			if (a >= 1.6f)
				return 0;
			return (1 - 1.259f * a + 0.396f * a * a) /
				(3.535f * a + 2.181f * a * a);
		}

	protected:
		Real alphaX, alphaY;
};

class TrowbridgeReitzDistribution : public MicrofacetDistribution {
	public:

};