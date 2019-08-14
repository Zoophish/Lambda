/*
Structure that provides access to necessary information needed for shading.
*/

#pragma once
#include <maths/maths.h>
#include <core/Ray.h>

struct SurfaceScatterEvent {
	Vec3 wo, wi;
	Vec2 uv;
	Real pdf, eta;
	RayHit *hit;
};