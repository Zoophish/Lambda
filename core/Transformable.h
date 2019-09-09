//----	Sam Warren 2019	----
//3x3 row-major transformation matrix plus a translation vector.

#pragma once
#include <maths/maths.h>

class Transformable {
	public:
		Affine3 xfm;

		Transformable() {}
		
		inline void SetPosition(const Vec3 &_pos) {
			xfm[9] = _pos.x;
			xfm[10] = _pos.y;
			xfm[11] = _pos.z;
		}

		inline void SetScale(const Vec3 &_scale) {
			xfm[0] = _scale.x;
			xfm[4] = _scale.y;
			xfm[8] = _scale.z;
		}

		inline void SetRotation(const Real _theta, const Real _phi) {

		}
};