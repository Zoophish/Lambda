/*----	By Sam Warren 2019	----
  ----	Embree scene implmentation as an interface for scene construction and ray queries.	----

	Scene flags improve results for certain tyes of scenes. If the scene flag is changed,
	the scene must be re-comitted to apply the changes.

	Uses one device per scene, hence the RTCDevice is kept here too. Scene constructor can
	take a config string which determines how Embree runs on the hardware. By default, this
	is NULL which leaves the device on default configuration. Specific device configurations
	can be found in the Embree manual.
*/

#pragma once
#include <vector>
#include <lighting/Light.h>
#include <sampling/Piecewise.h>
#include "Object.h"
#include <lighting/EnvironmentLight.h>

class Scene {
	protected:
		RTCScene scene;

	public:
		RTCDevice device;
		std::vector<Object*> objects;
		std::vector<Light*> lights;
		EnvironmentLight *envLight;
		Distribution::Piecewise1D lightDistribution;

		Scene(const RTCSceneFlags _sceneFlags = RTC_SCENE_FLAG_NONE, const char* _deviceConfig = NULL) {
			device = rtcNewDevice(_deviceConfig);
			scene = rtcNewScene(device);
			SetFlags(_sceneFlags);
		}

		inline void SetFlags(const RTCSceneFlags _flags = RTC_SCENE_FLAG_NONE) {
			rtcSetSceneFlags(scene, _flags);
		}

		void Commit(const RTCBuildQuality _buildQuality = RTC_BUILD_QUALITY_HIGH) {
			rtcSetSceneBuildQuality(scene, _buildQuality);
			rtcCommitScene(scene);
			UpdateLightDistribution();
		}

		bool Intersect(const Ray &_ray, RayHit &_hit) const {
			RTCRay eRay = _ray.ToRTCRay();
			RTCRayHit rayHit;
			rayHit.ray = eRay;
			RTCIntersectContext context;
			rtcInitIntersectContext(&context);
			rtcIntersect1(scene, &context, &rayHit);
			if(rayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID && rayHit.ray.tfar > 0 && rayHit.ray.tfar < INFINITY) {
				_hit = objects[rayHit.hit.geomID]->Hit(rayHit);
				_hit.object = objects[rayHit.hit.geomID];
				_hit.primId = rayHit.hit.primID;
				return true;
			}
			return false;
		}

		bool MutualVisibility(const Vec3 &_p1, const Vec3 &_p2) const {
			const Vec3 diff = _p2 - _p1;
			const Real mag = diff.Magnitude();
			const Vec3 dir = diff / mag;
			RTCRay eRay = Ray(_p1 + dir * .0005, dir).ToRTCRay();
			RTCRayHit rayHit;
			rayHit.ray = eRay;
			RTCIntersectContext context;
			rtcInitIntersectContext(&context);
			rtcIntersect1(scene, &context, &rayHit);
			return rayHit.ray.tfar > (mag - 0.001f);
		}

		bool RayEscapes(const Ray &_ray) const {
			RTCRay eRay = _ray.ToRTCRay();
			RTCIntersectContext context;
			rtcInitIntersectContext(&context);
			rtcOccluded1(scene, &context, &eRay);
			return eRay.tfar == INFINITY;
		}

		void AddObject(Object &_obj) {
			rtcCommitGeometry(_obj.geometry);
			rtcAttachGeometryByID(scene, _obj.geometry, objects.size());
			objects.push_back(&_obj);
		}

		void RemoveObject(const unsigned _i) {
			rtcDetachGeometry(scene, objects[_i]->geometryId);
			objects.erase(objects.begin() + _i);
		}

		void AddLight(Light *_light) {
			lights.push_back(_light);
		}

		inline Bounds GetBounds() const {
			RTCBounds b;
			rtcGetSceneBounds(scene, &b);
			return Bounds(Vec3(b.lower_x, b.lower_y, b.lower_z), Vec3(b.upper_x, b.upper_y, b.upper_z));
		}

		void UpdateLightDistribution() {
			if (envLight) {
				RTCBounds b;
				rtcGetSceneBounds(scene, &b);
				envLight->diameter = b.upper_x - b.lower_x;
			}
			std::vector<Real> importances(lights.size());
			for (unsigned i = 0; i < lights.size(); ++i) {
				importances[i] = lights[i]->Power();
			}
			lightDistribution = Distribution::Piecewise1D(&importances[0], lights.size());
		}
};