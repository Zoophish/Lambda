//----	By Sam Warren 2019	----
//----	Spherical view projected onto a 2D plane for environment maps, etc.	----

#pragma once
#include <camera/Camera.h>

class SphericalCamera : public Camera {
	public:
		Real offsetPhi, offsetTheta;

		SphericalCamera(const Vec3& _origin, const Real _offsetPhi = 0, const Real _offsetTheta = 0) {
			origin = _origin;
			offsetPhi = _offsetPhi;
			offsetTheta = _offsetTheta;
		}

		Ray GenerateRay(const Real _u, const Real _v) const override {
			const Real phi = PI2 * _u + offsetPhi;
			const Real theta = PI * _v + offsetTheta;
			return Ray(origin, maths::SphericalDirection(std::sin(theta), std::cos(theta), phi).Normalised());
		}
};