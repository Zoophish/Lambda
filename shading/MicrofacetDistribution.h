#pragma once
#include "graph/ShaderGraph.h"
#include "surface/BxDF.h"

LAMBDA_BEGIN

class MicrofacetDistribution {
	public:
		virtual Real D(const Vec3 &_w, const Vec2 &_alpha) const = 0;

		virtual Real Lambda(const Vec3 &_w, const Vec2 &_alpha) const = 0;

		virtual Vec3 Sample_wh(Sampler &_sampler, const Vec3 &_wo, const Vec2 &_alpha) const = 0;

		inline Real Pdf(const Vec3 &_wh, const Vec2 &_alpha) const {
			return D(_wh, _alpha) * std::abs(_wh.y);
		}
		
		inline Real G1(const Vec3 &_w, const Vec2 &_alpha) const {
			return (Real)1 / ((Real)1 + Lambda(_w, _alpha));
		}

		inline Real G(const Vec3 &_wo, const Vec3 &_wi, const Vec2 &_alpha) const {
			return (Real)1 / ((Real)1 + Lambda(_wo, _alpha) + Lambda(_wi, _alpha));
		}
};



class BeckmannDistribution : public MicrofacetDistribution {
	public:
		BeckmannDistribution();

		Real D(const Vec3 &_wh, const Vec2 &_alpha) const override;

		Real Lambda(const Vec3 &_w, const Vec2 &_alpha) const override;

		Vec3 Sample_wh(Sampler &_sampler, const Vec3 &_wo, const Vec2 &_alpha) const override;
};



class TrowbridgeReitzDistribution : public MicrofacetDistribution {
	public:
		TrowbridgeReitzDistribution();

		Real D(const Vec3 &_wh, const Vec2 &_alpha) const override;

		Real Lambda(const Vec3 &_w, const Vec2 &_alpha) const override;

		Vec3 Sample_wh(Sampler &_sampler, const Vec3 &_wo, const Vec2 &_alpha) const override;
};



//class GGXDistribution : public MicrofacetDistribution {
//	public:
//		GGXDistribution();
//
//		Real D(const Vec3 &_wh, const Vec2 &_alpha) const override;
//
//		Real Lambda(const Vec3 &_w, const Vec2 &_alpha) const override;
//
//		Vec3 Sample_wh(Sampler &_sampler, const Vec3 &_wo, const Vec2 &_alpha) const override;
//};

LAMBDA_END