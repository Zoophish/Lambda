//----	By Sam Warren 2019	----
//----	Base class for camera models.	----

#pragma once
#include <core/Ray.h>

class Camera {
	public:
		Real aperture, shutterSpeed;
		Vec3 origin;
		//Film film

		Camera(const Real _x = 1, const Real _y = 1) {
			aspect = _y / _x;
		}

		Camera(const unsigned _x, const unsigned _y) {
			aspect = (float)_y / (float)_x;
		}

		virtual Ray GenerateRay(const Real _u, const Real _v) const = 0;

	protected:
		Real aspect;
};