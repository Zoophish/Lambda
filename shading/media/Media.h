#pragma once
#include "HenyeyGreenstein.h"
#include <core/Scene.h>
#include "../ScatterEvent.h"

LAMBDA_BEGIN

class Medium {
	public:
		PhaseFunction *phase;

		virtual Spectrum Tr(const Ray &_ray, const Real _tFar, Sampler &_sampler) const = 0;

		virtual Spectrum Sample(const Ray &_ray, Sampler &_sampler, ScatterEvent &_event) const = 0;
};

class MediaBoundary {
	public:
		Medium *interior, *exterior;	//exterior does not adhere to closed mesh rule... should always be nullptr

		MediaBoundary(Medium *_interior = nullptr, Medium *_exterior = nullptr) {
			interior = _interior;
			exterior = _exterior;
		}

		inline Medium *GetMedium(const Vec3 &_w, const Vec3 &_n) const {
			return maths::Dot(_w, _n) < 0 ? interior : exterior;
		}
};

LAMBDA_END