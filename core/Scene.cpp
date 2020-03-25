#include "Scene.h"

LAMBDA_BEGIN

Scene::Scene(const RTCSceneFlags _sceneFlags, const char *_deviceConfig) {
	device = rtcNewDevice(_deviceConfig);
	scene = rtcNewScene(device);
	SetFlags(_sceneFlags);
	hasVolumes = false;
}

bool Scene::Intersect(const Ray &_ray, RayHit &_hit) const {
	RTCRayHit rayHit;
	rayHit.ray = _ray.ToRTCRay();
	RTCIntersectContext context;
	rtcInitIntersectContext(&context);
	context.flags = RTC_INTERSECT_CONTEXT_FLAG_INCOHERENT;
	rtcIntersect1(scene, &context, &rayHit);
	if (rayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID && rayHit.ray.tfar > 0 && rayHit.ray.tfar < INFINITY) {
		objects[rayHit.hit.geomID]->Hit(rayHit, _hit);
		if (rayHit.hit.instID[0] != RTC_INVALID_GEOMETRY_ID) _hit.object = objects[rayHit.hit.instID[0]];
		else _hit.object = objects[rayHit.hit.geomID];
		_hit.primId = rayHit.hit.primID;
		return true;
	}
	return false;
}

bool Scene::IntersectTr(Ray _r, RayHit &_hit, Sampler &_sampler, Medium *_med, Spectrum *_Tr) const {
	Real tFar = 0;
	while (Intersect(_r, _hit)) {
		tFar += _hit.tFar;
		if (_med && _Tr) *_Tr *= _med->Tr(_r, _hit.tFar, _sampler);
		if (_hit.object->bxdf || _hit.object->light) {
			_hit.tFar = tFar;
			return true;
		}
		_med = _hit.object->mediaBoundary->GetMedium(_r.d, _hit.normalG);
		_r.o = _hit.point + _hit.normalG *(maths::Dot(_hit.normalG, _r.d) < 0 ? -.0001 : .0001);
	}
	//if (_med) _Tr = 0;
	_hit.tFar = tFar;
	return false;
}

bool Scene::MutualVisibility(const Vec3 &_p1, const Vec3 &_p2, Vec3 *_w) const {
	const Vec3 diff = _p2 - _p1;
	const Real mag = diff.Magnitude();
	const Vec3 dir = diff / mag;
	RTCRay eRay = Ray(_p1, dir).ToRTCRay();
	eRay.tfar = mag - .00001;
	RTCIntersectContext context;
	rtcInitIntersectContext(&context);
	context.flags = RTC_INTERSECT_CONTEXT_FLAG_INCOHERENT;
	rtcOccluded1(scene, &context, &eRay);
	if (_w) *_w = dir;
	return eRay.tfar != -INFINITY;
}

bool Scene::MutualVisibility(const Vec3 &_p1, const Vec3 &_p2) const {
	return MutualVisibility(_p1, _p2, nullptr);
}

bool Scene::RayEscapes(const Ray &_ray) const {
	RTCRay eRay = _ray.ToRTCRay();
	RTCIntersectContext context;
	rtcInitIntersectContext(&context);
	rtcOccluded1(scene, &context, &eRay);
	return eRay.tfar != -INFINITY;
}

void Scene::AddObject(Object *_obj, const bool _addLight) {
	_obj->Commit(device);
	rtcAttachGeometryByID(scene, _obj->geometry, objects.size());
	objects.push_back(_obj);
	if (_addLight && _obj->light) {
		AddLight(_obj->light);
	}
}

void Scene::UpdateLightDistribution() {
	if (envLight) {
		RTCBounds b;
		rtcGetSceneBounds(scene, &b);
		const Real xd = b.upper_x - b.lower_x;
		const Real yd = b.upper_y - b.lower_y;
		const Real zd = b.upper_z - b.lower_z;
		envLight->radius = std::max(std::max(xd, yd), zd) * .5;
	}
	std::unique_ptr<Real[]> importances(new Real[lights.size()]);
	for (unsigned i = 0; i < lights.size(); ++i) {
		importances[i] = lights[i]->Power();
	}
	lightDistribution = Distribution::Piecewise1D(&importances[0], lights.size());
}

LAMBDA_END