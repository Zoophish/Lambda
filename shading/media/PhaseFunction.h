#pragma once
#include <maths/maths.h>

class PhaseFunction {
	public:
		virtual Real p(const Vec3 &_wo, const Vec3 &_wi) const = 0;

		virtual Real Sample_p(const Vec3 &_wo, Vec3 *_wi, const Vec2 &_u) const = 0;
};