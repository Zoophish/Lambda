#pragma once
#include "Ray.h"
class BxDF;

class Object {
	public:
		RTCGeometry geometry;
		unsigned geometryId;
		BxDF *bxdf;
		Light *light = nullptr;

		Object() {}

		RayHit Hit(const RTCRayHit &_h) const {
			RayHit hit;
			hit.point.x = _h.ray.org_x + _h.ray.dir_x * _h.ray.tfar;
			hit.point.y = _h.ray.org_y + _h.ray.dir_y * _h.ray.tfar;
			hit.point.z = _h.ray.org_z + _h.ray.dir_z * _h.ray.tfar;
			hit.normalG = Vec3(_h.hit.Ng_x, _h.hit.Ng_y, _h.hit.Ng_z).Normalised();
			ProcessHit(hit, _h);
			return hit;
		}

	protected:
		virtual void ProcessHit(RayHit &_hit, const RTCRayHit &_h) const = 0;
};