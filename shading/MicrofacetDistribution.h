#pragma once
#include <maths/maths.h>
#include "BxDF.h"

class MicrofacetDistribution {
	public:
		virtual Real D(const Vec3 &_w) const = 0;

		virtual Real Lambda(const Vec3 &_w) const = 0;

		virtual Vec3 Sample_wh(const Vec2 &_u, const Vec3 &_wo) const = 0;

		Real Pdf(const Vec3 &_wh) const { return D(_wh) * std::abs(_wh.y); }
		
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
			if (a >= 1.6f) return 0;
			return (1 - 1.259f * a + 0.396f * a * a) / (3.535f * a + 2.181f * a * a);
		}

		Vec3 Sample_wh(const Vec2 &_u, const Vec3 &_wo) const override {
			if (alphaX == alphaY) {
				const Real log = _u.x == 1 ? 0 : std::log(1 - _u.x);
				//const Real theta = std::atan(std::sqrt(-alphaX * alphaX * log));
				const Real phi = PI2 * _u.y;
				const Real tan2Theta = -alphaX * alphaX * log;
				const Real cosTheta = 1 / std::sqrt(1 + tan2Theta);
				const Real sinTheta = std::sqrt(std::max((Real)0, 1 - cosTheta * cosTheta));
				Vec3 h =  maths::SphericalDirection(sinTheta, cosTheta, phi);
				return !SameHemisphere(_wo, h) ? -h : h;
			}
			else {

			}
		}

	protected:
		Real alphaX, alphaY;
};

class GGXDistribution : public MicrofacetDistribution {
	public:
		
};