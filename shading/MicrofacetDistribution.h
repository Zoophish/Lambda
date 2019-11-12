#pragma once
#include "surface/BxDF.h"

class MicrofacetDistribution {
	public:
		virtual Real D(const Vec3 &_w) const = 0;

		virtual Real Lambda(const Vec3 &_w) const = 0;

		virtual Vec3 Sample_wh(const Vec2 &_u, const Vec3 &_wo) const = 0;

		inline Real Pdf(const Vec3 &_wh) const {
			return D(_wh) * std::abs(_wh.y);
		}
		
		inline Real G1(const Vec3 &_w) const {
			return (Real)1 / ((Real)1 + Lambda(_w));
		}

		inline Real G(const Vec3 &_wo, const Vec3 &_wi) const {
			return (Real)1 / ((Real)1 + Lambda(_wo) + Lambda(_wi));
		}

		static inline Real RoughnessToAlpha(Real _roughness) {
			_roughness = std::max(_roughness, (Real)1e-3);
			Real x = std::log(_roughness);
			return 1.62142f + 0.819955f * x + 0.1734f * x * x +
				0.0171201f * x * x * x + 0.000640711f * x * x * x * x;
		}
};

class BeckmannDistribution : public MicrofacetDistribution {
	public:
		BeckmannDistribution(const Real _alphaX, const Real _alphaY);

		inline void SetSigmas(const Real _sigmaX, const Real _sigmaY) {
			alphaX = std::sqrt(2 * _sigmaX);
			alphaY = std::sqrt(2 * _sigmaY);
		}

		Real D(const Vec3 &_wh) const override;

		Real Lambda(const Vec3 &_w) const override;

		Vec3 Sample_wh(const Vec2 &_u, const Vec3 &_wo) const override;

	protected:
		Real alphaX, alphaY;
};

class TrowbridgeReitzDistribution : public MicrofacetDistribution {
	public:
		TrowbridgeReitzDistribution(const Real _alphaX, const Real _alphaY);

		Real D(const Vec3 &_wh) const override;

		Real Lambda(const Vec3 &_w) const override;

		Vec3 Sample_wh(const Vec2 &_u, const Vec3 &_wo) const override;

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