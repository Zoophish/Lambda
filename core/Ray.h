/*----	By Sam Warren 2019	----
  ----	Basic ray class and ray-hit struct.	----

	Since Embree only supports 32-bit float precision, vec3f used as Vec3 in maths.h.
	Can be converted to an Embree RTCRay type when querying ray against a scene.
*/

#pragma once
#include <maths/maths.h>

class Ray {
	public:
		Vec3 o, d;

		Ray() {}

		Ray(const Vec3 &_o, const Vec3 &_d) {
			o = _o;
			d = _d;
		}

		inline RTCRay ToRTCRay() const {
			RTCRay ray;
			ray.dir_x = d.x;
			ray.dir_y = d.y;
			ray.dir_z = d.z;
			ray.org_x = o.x;
			ray.org_y = o.y;
			ray.org_z = o.z;
			ray.tfar = INFINITY;
			ray.tnear = 0;
			ray.mask = 0xFFFFFFFF;
			ray.time = 0;
			ray.id = 0;
			ray.flags = 0;
		}

		inline Vec3 operator()(const float _t) const { return o + d * _t; }
};

struct RayHit {
	Vec3 point, normal, tangent, bitangent;
	Vec2 uvCoords;
	unsigned geomId;
};