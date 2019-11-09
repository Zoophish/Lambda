#include "Scene.h"

Scene::Scene(const RTCSceneFlags _sceneFlags, const char *_deviceConfig) {
	device = rtcNewDevice(_deviceConfig);
	scene = rtcNewScene(device);
	SetFlags(_sceneFlags);
}

bool Scene::Intersect(const Ray &_ray, RayHit &_hit) const {
	RTCRay eRay = _ray.ToRTCRay();
	RTCRayHit rayHit;
	rayHit.ray = eRay;
	RTCIntersectContext context;
	rtcInitIntersectContext(&context);
	context.flags = RTC_INTERSECT_CONTEXT_FLAG_INCOHERENT;
	rtcIntersect1(scene, &context, &rayHit);
	if (rayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID && rayHit.ray.tfar > 0 && rayHit.ray.tfar < INFINITY) {
		_hit.tFar = rayHit.ray.tfar;
		_hit = objects[rayHit.hit.geomID]->Hit(rayHit);
		_hit.object = objects[rayHit.hit.geomID];
		if (rayHit.hit.instID[0] != RTC_INVALID_GEOMETRY_ID) {
			_hit.object = objects[rayHit.hit.instID[0]];
		}
		_hit.primId = rayHit.hit.primID;
		return true;
	}
	return false;
}

bool Scene::MutualVisibility(const Vec3 &_p1, const Vec3 &_p2) const {
	const Vec3 diff = _p2 - _p1;
	const Real mag = diff.Magnitude();
	const Vec3 dir = diff / mag;
	RTCRay eRay = Ray(_p1, dir).ToRTCRay();
	RTCRayHit rayHit;
	rayHit.ray = eRay;
	RTCIntersectContext context;
	rtcInitIntersectContext(&context);
	context.flags = RTC_INTERSECT_CONTEXT_FLAG_INCOHERENT;
	rtcIntersect1(scene, &context, &rayHit);
	return rayHit.ray.tfar > (mag - 0.002);
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