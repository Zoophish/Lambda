#pragma once
#include "PhaseFunction.h"

LAMBDA_BEGIN

class HenyeyGreenstein : public PhaseFunction {
	public:
		Real g;

		HenyeyGreenstein(const Real _g = 0);

		Real p(const Vec3 &_wo, const Vec3 &_wi) const override;

		Real Sample_p(const Vec3 &_wo, Vec3 *_wi, Sampler &_sampler) const override;
};

LAMBDA_END