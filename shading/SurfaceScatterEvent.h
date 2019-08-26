/*
Structure that provides access to necessary information needed for shading.
*/


//TO DO: Rename to SurfaceEvent.
#pragma once
#include <maths/maths.h>
#include <core/Ray.h>
#include <lighting/Light.h>
class Scene;
class BxDF;

struct SurfaceScatterEvent {
	bool mode = 0;
	Vec3 wo, wi;
	Real pdf, eta;
	RayHit *hit;
	const Scene *scene;

	void ToSurfaceSpace() {

	}

	void ToWorldSpace() {

	}
};