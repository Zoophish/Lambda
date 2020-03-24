#pragma once
#include "PhaseFunction.h"

LAMBDA_BEGIN

namespace Media {

	static inline Real PhaseHG(const Real _cosTheta, const Real _g) {
		const Real g2 = _g * _g;
		const Real denom = 1 + g2 + 2 * _g * _cosTheta;
		return INV_PI4 * (1 - g2) / (denom * std::sqrt(denom));
	}

}

template <class T> inline void
CoordinateSystem(const vec3<T> &v1, vec3<T> *v2, vec3<T> *v3) {
	if (std::abs(v1.x) > std::abs(v1.y))
		*v2 = vec3<T>(-v1.z, 0, v1.x) / std::sqrt(v1.x * v1.x + v1.z * v1.z);
	else
		*v2 = vec3<T>(0, v1.z, -v1.y) / std::sqrt(v1.y * v1.y + v1.z * v1.z);
	*v3 = maths::Cross(v1, *v2);
}

class HenyeyGreenstein : public PhaseFunction {
	public:
		Real g;

		HenyeyGreenstein() {
			g = 0;
		}

		Real p(const Vec3 &_wo, const Vec3 &_wi) const override {
			return Media::PhaseHG(maths::Dot(_wo, _wi), g);
		}

		Real Sample_p(const Vec3 &_wo, Vec3 *_wi, Sampler &_sampler) const override {
			Real cosTheta;
			const Vec2 u = _sampler.Get1D();
			if (std::abs(g) < 1e-3) cosTheta = 1 - 2 * u.x;
			else {
				const Real sqrTerm = (1 - g * g) / (1 - g + 2 * g * u.x);
				cosTheta = (1 + g * g - sqrTerm * sqrTerm) / (2 * g);
			}
			const Real sinTheta = std::sqrt(std::max((Real)0, 1 - cosTheta * cosTheta));
			const Real phi = 2 * PI * u.y;
			//const Vec3 v1 = maths::Cross(-_wo, Vec3(0, 1, 0));
			//const Vec3 v2 = maths::Cross(-_wo, v1);
			Vec3 v1, v2;
			CoordinateSystem(_wo, &v1, &v2);
			const Vec3 d = maths::SphericalDirection(sinTheta, cosTheta, phi);
			*_wi = d;// v1 *d.x + -_wo * d.y + v2 * d.z;
			return Media::PhaseHG(-cosTheta, g);
		}
};

LAMBDA_END