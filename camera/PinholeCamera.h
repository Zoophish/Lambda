/*----	By Sam Warren 2019	----
  ----	Basic pinhole camera.	----

	Camera controls are origin, look direction and field of view.
	Changes to camera paramteres made through functions so that expensive information
	can be cached, so ray generation faster.

*/
#pragma once
#include <algorithm>
#include "Camera.h"

class PinholeCamera : public Camera {
	public:
		PinholeCamera(const Vec3 &_origin, const float _x, const float _y) : Camera(_x, _y) {
			origin = _origin;
			fov = 1.1;
			CalculateBasis();
			CalculateTrig();
		}

		inline void SetRotation(const float _phi, const float _theta) {
			phi = _phi;
			theta = _theta;
			CalculateBasis();
		}

		inline void SetForwards(const Vec3 &_forwards) {
			zHat = _forwards.Normalised();
			xHat = Vec3::Cross(zHat, Vec3(0, 1, 0));
			yHat = Vec3::Cross(zHat, xHat);
		}

		inline void SetFov(const float _fov) {
			fov = _fov;
			CalculateTrig();
		}

		Ray GenerateRay(const float _u, const float _v) const override {
			const Vec3 p = xHat*(_v*-tanFov2+tanFov) + yHat*(_v*-tanFov2*aspect + tanFov*aspect) + zHat;
			return Ray(origin, (p - origin).Normalised());
		}

	protected:
		Vec3 xHat, yHat, zHat;
		float phi, theta, tanFov, tanFov2, fov;

		inline void CalculateBasis() {
			zHat = Vec3(std::cos(theta) * std::sin(phi), std::sin(theta), std::cos(theta) * std::cos(phi)).Normalised();
			xHat = Vec3::Cross(zHat, Vec3(0, 1, 0));
			yHat = Vec3::Cross(zHat, xHat);
		}

		inline void CalculateTrig() {
			tanFov = std::tan(.5 * fov);
			tanFov2 = 2 * tanFov;
		}
};