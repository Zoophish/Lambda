#include "Light.h"
#include <core/Scene.h>

LAMBDA_BEGIN

bool Light::MutualVisibility(const Vec3 &_p1, const Vec3 &_p2, ScatterEvent &_event, const Scene &_scene, Sampler &_sampler, Spectrum *_Tr) {
	if (_scene.hasVolumes) {
		const Vec3 diff = _p2 - _p1;
		const Real mag = diff.Magnitude();
		const Vec3 dir = diff / mag;
		_event.wi = dir;
		const Real minT = mag - SURFACE_EPSILON * 10;	//10 'null intersections' - tFar is reduced by SURFACE_EPSILON every null bounce
		RayHit hit;
		Medium *med = _event.medium;
		Ray r(_p1, dir);
		if (_scene.IntersectTr(r, hit, _sampler, med, _Tr)) {
			return hit.tFar > minT;
		}
		return false;
	}
	return _scene.MutualVisibility(_p1, _p2, &_event.wi);
}

bool Light::RayEscapes(const Ray &_r, const ScatterEvent &_event, Sampler &_sampler, Spectrum *_Tr) {
	if (_event.scene->hasVolumes) {
		RayHit hit;
		Medium *med = _event.medium;
		return !_event.scene->IntersectTr(_r, hit, _sampler, med, _Tr);
	}
	return _event.scene->RayEscapes(_r);
}

LAMBDA_END