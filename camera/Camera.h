//----	By Sam Warren 2019	----
//----	Base class for camera models.	----

#pragma once
#include <core/Ray.h>

class Camera {
	public:
		float aperture, shutterSpeed;
		Vec3 origin;
		//Film film

		Camera(const float _x = 1, const float _y = 1) {
			aspect = _y / _x;
		}

		Camera(const unsigned _x, const unsigned _y) {
			aspect = (float)_y / (float)_x;
		}

		virtual Ray GenerateRay(const float _u, const float _v) const = 0;

	protected:
		float aspect;
};