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
#include "Object.h"
#include <lighting/Light.h>
#include <sampling/Piecewise.h>
#include <shading/media/Media.h>

LAMBDA_BEGIN

class Scene {
	protected:
		RTCScene scene;

	public:
		RTCDevice device;
		std::vector<Object*> objects; //Root(s) of object tree.
		std::vector<Light*> lights;
		Light* envLight;
		Distribution::Piecewise1D lightDistribution;
		bool hasVolumes;

		Scene(const RTCSceneFlags _sceneFlags = RTC_SCENE_FLAG_NONE, const char *_deviceConfig = NULL);

		/*
			Sets Embree scene flags.
		*/
		inline void SetFlags(const RTCSceneFlags _flags = RTC_SCENE_FLAG_NONE) {
			rtcSetSceneFlags(scene, _flags);
		}

		/*
			Builds scene's accelleration structure and lighting distribution.
		*/
		inline void Commit(const RTCBuildQuality _buildQuality = RTC_BUILD_QUALITY_HIGH) {
			rtcSetSceneBuildQuality(scene, _buildQuality);
			rtcCommitScene(scene);
			UpdateLightDistribution();
		}

		/*
			Queries _ray against scene geometry.
				- Returns true if intersection found.
				- Hit information passed to _hit.
		*/
		bool Intersect(const Ray &_ray, RayHit &_hit) const;

		/*
			Queries _ray against scene geometry, ignoring pure volumes and returning beam transmittance to _Tr.
		*/
		bool IntersectTr(Ray _r, RayHit &_hit, Sampler &_sampler, Medium *_med, Spectrum *_Tr = nullptr) const;

		/*
			Returns true if points _p1 and _p2 are mutually visible against scene geometry.
		*/
		bool MutualVisibility(const Vec3 &_p1, const Vec3 &_p2, Vec3 *_w) const;
		bool MutualVisibility(const Vec3 &_p1, const Vec3 &_p2) const;

		/*
			Returns true if _ray intersects no geometry within the scene.
		*/
		bool RayEscapes(const Ray &_ray) const;

		/*
			Explicitly adds _light to the lighting distribution without adding
			intersectable geometry.
		*/
		inline void AddLight(Light *_light) {
			lights.push_back(_light);
		}

		/*
			Commits changes to _obj's geometry and adds to scene geometry.
			- _addLight will automatically add _obj's light (if any) to the lighting distribution.
		*/
		void AddObject(Object *_obj, const bool _addLight = true);

		/*
			Removes the object indexed by _i from scene.
		*/
		inline void RemoveObject(const unsigned _i) {
			rtcDetachGeometry(scene, _i);
			objects.erase(objects.begin() + _i);
		}

		/*
			Returns bounding box of scene's geometry.
		*/
		inline Bounds GetBounds() const {
			RTCBounds b;
			rtcGetSceneBounds(scene, &b);
			return Bounds(Vec3(b.lower_x, b.lower_y, b.lower_z), Vec3(b.upper_x, b.upper_y, b.upper_z));
		}

		/*
			Commits changes to scene lighting ready for rendering.
				- Automatically called when scene is committed.
				- Call if lights have been changed but geometry hasn't.
		*/
		void UpdateLightDistribution();
};

struct SceneNode {
	std::string name;
	SceneNode *parent = nullptr;
	Affine3 xfm;
	std::vector<Object*> objects;
	std::vector<SceneNode> children;
};

LAMBDA_END