#pragma once
#include <Lambda.h>
#include <maths/maths.h>

LAMBDA_BEGIN

namespace Sampling {

	static inline Vec2 SampleUnitDisk(const Vec2 &_u) {
		const Real r = std::sqrt(_u.x);
		const Real theta = _u.y * PI2;
		return Vec2(r * cos(theta), r * sin(theta));
	}

	static inline Vec3 SampleCosineHemisphere(const Vec2 &_u) {
		const Vec2 d = SampleUnitDisk(_u);
		const Real up = std::sqrt(std::max((Real)0, 1 - d.x * d.x - d.y * d.y));
		return Vec3(d.x, up, d.y);
	}

}

LAMBDA_END