/*
Structure that provides access to necessary information needed for shading.
*/


//TO DO: Rename to SurfaceEvent.
#pragma once
#include <maths/maths.h>
#include <core/Ray.h>
class BxDF;
class Scene;

struct SurfaceScatterEvent {
	Vec3 wo, wi;
	Real pdf, eta;
	RayHit *hit;
	const Scene *scene;

	inline Vec3 ToLocal(const Vec3 &_v) const {
		if(hit) return maths::ToSpace(_v, hit->normalS, hit->tangent, hit->bitangent);
	}
	
	inline Vec3 ToWorld(const Vec3 &_v) const {
		if(hit) return maths::FromSpace(_v, hit->normalS, hit->tangent, hit->bitangent);
	}
};