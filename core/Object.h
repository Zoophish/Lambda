#pragma once
#include <embree3/rtcore.h>
#include "Ray.h"

class Object {
	public:
		RTCGeometry geometry;
		unsigned geometryId;

		Object() {}

		RayHit Hit(const RTCRayHit &_h) const {
			RayHit hit;
			hit.geomId = geometryId;
			hit.point.x = _h.ray.org_x + _h.ray.dir_x * _h.ray.tfar;
			hit.point.y = _h.ray.org_y + _h.ray.dir_y * _h.ray.tfar;
			hit.point.z = _h.ray.org_z + _h.ray.dir_z * _h.ray.tfar;
			ProcessHit(hit, _h);
			return hit;
		}

	protected:
		virtual void ProcessHit(RayHit &_hit, const RTCRayHit &_h) const = 0;
};