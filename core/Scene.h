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
#include <embree3/rtcore.h>
#include "Object.h"

class Scene {
	protected:
		RTCDevice device;
		RTCScene scene;

	public:
		std::vector<Object*> objects;

		Scene(const RTCSceneFlags _sceneFlags = RTC_SCENE_FLAG_NONE, const char* _deviceConfig = NULL) {
			device = rtcNewDevice(_deviceConfig);
			scene = rtcNewScene(device);
			SetFlags(_sceneFlags);
		}

		inline void SetFlags(const RTCSceneFlags _flags = RTC_SCENE_FLAG_NONE) {
			rtcSetSceneFlags(scene, _flags);
		}

		void AddObject(Object &_obj) {
			rtcCommitGeometry(_obj.geometry);
			_obj.geometryId = rtcAttachGeometry(scene, _obj.geometry);
			objects.push_back(&_obj);
		}

		void RemoveObject(const unsigned _i) {
			rtcDetachGeometry(scene, objects[_i]->geometryId);
			objects.erase(objects.begin() + _i);
		}

		void Commit(const RTCBuildQuality _buildQuality = RTC_BUILD_QUALITY_HIGH) {
			rtcSetSceneBuildQuality(scene, _buildQuality);
			rtcCommitScene(scene);
		}

		bool Intersect(const Ray &ray, RayHit &hit) const {
			RTCRay eRay = ray.ToRTCRay();
			RTCRayHit rayHit;
			rayHit.ray = eRay;
			RTCIntersectContext context;
			rtcInitIntersectContext(&context);
			rtcIntersect1(scene, &context, &rayHit);
			if(rayHit.ray.tfar > 0 && rayHit.ray.tfar < INFINITY) {
				hit = objects[rayHit.hit.geomID]->Hit(rayHit);
				return true;
			}
			return false;
		}
};