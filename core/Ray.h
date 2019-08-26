/*----	By Sam Warren 2019	----
  ----	Basic ray class and ray-hit struct.	----

	Since Embree only supports 32-bit float precision, vec3f used as Vec3 in maths.h.
	Can be converted to an Embree RTCRay type when querying ray against a scene.
*/

#pragma once
#include <maths/maths.h>
#include <embree3/rtcore.h>
class Object;

class Ray {
	public:
		Real eta;
		Vec3 o, d;

		Ray() {}

		Ray(const Vec3 &_o, const Vec3 &_d) {
			o = _o;
			d = _d;
		}

		RTCRay ToRTCRay() const {
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
			return ray;
		}

		inline Vec3 operator()(const Real _t) const { return o + d * _t; }
};

struct RayHit {
	Vec3 point, normalG, normalS, tangent, bitangent;
	Vec2 uvCoords;
	Object *object;
	unsigned primId;
};