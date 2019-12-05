#pragma once
#include "graph/ShaderGraph.h"
#include "surface/BxDF.h"

class MicrofacetDistribution {
	public:
		virtual Real D(const Vec3 &_w, const Vec2 &_alpha) const = 0;

		virtual Real Lambda(const Vec3 &_w, const Vec2 &_alpha) const = 0;

		virtual Vec3 Sample_wh(const Vec2 &_u, const Vec3 &_wo, const Vec2 &_alpha) const = 0;

		inline Real Pdf(const Vec3 &_wh, const Vec2 &_alpha) const {
			return D(_wh, _alpha) * std::abs(_wh.y);
		}
		
		inline Real G1(const Vec3 &_w, const Vec2 &_alpha) const {
			return (Real)1 / ((Real)1 + Lambda(_w, _alpha));
		}

		inline Real G(const Vec3 &_wo, const Vec3 &_wi, const Vec2 &_alpha) const {
			return (Real)1 / ((Real)1 + Lambda(_wo, _alpha) + Lambda(_wi, _alpha));
		}

		static inline Vec2 RoughnessToAlpha(Real _roughness) {
			_roughness = std::max(_roughness, (Real)1e-3);
			Real x = std::log(_roughness);
			x = 1.62142f + 0.819955f * x + 0.1734f * x * x +
				0.0171201f * x * x * x + 0.000640711f * x * x * x * x;
			return Vec2(x, x);
		}
};



class BeckmannDistribution : public MicrofacetDistribution {
	public:
		BeckmannDistribution();

		Real D(const Vec3 &_wh, const Vec2 &_alpha) const override;

		Real Lambda(const Vec3 &_w, const Vec2 &_alpha) const override;

		Vec3 Sample_wh(const Vec2 &_u, const Vec3 &_wo, const Vec2 &_alpha) const override;
};



class TrowbridgeReitzDistribution : public MicrofacetDistribution {
	public:
		TrowbridgeReitzDistribution();

		Real D(const Vec3 &_wh, const Vec2 &_alpha) const override;

		Real Lambda(const Vec3 &_w, const Vec2 &_alpha) const override;

		Vec3 Sample_wh(const Vec2 &_u, const Vec3 &_wo, const Vec2 &_alpha) const override;
};



//class GGXDistribution : public MicrofacetDistribution {
//	public:
//		//GGXDistribution(const Real _sigmaX, const Real _sigmaY);
//		GGXDistribution();
//
//		Real D(const Vec3 &_wh, const Vec2 &_alpha) const override;
//
//		Real Lambda(const Vec3 &_w, const Vec2 &_alpha) const override;
//
//		Vec3 Sample_wh(const Vec2 &_u, const Vec3 &_wo, const Vec2 &_alpha) const override;
//};