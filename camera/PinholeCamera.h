/*----	By Sam Warren 2019	----
  ----	Basic pinhole camera.	----

	Camera controls are origin, look direction and field of view.
	Changes to camera paramteres made through functions so that expensive information
	can be cached, so ray generation faster.

*/
#pragma once
#include <camera/Camera.h>

class PinholeCamera : public Camera {
	public:
		PinholeCamera(const Vec3 &_origin, const float _x, const float _y) : Camera(_x, _y) {
			origin = _origin;
			SetFov(1.2);
			SetRotation(0, 0);
		}

		inline void SetRotation(const float _phi, const float _theta) {
			zHat = Vec3(std::cos(_theta) * std::sin(_phi), std::sin(_theta), std::cos(_theta) * std::cos(_phi)).Normalised();
			xHat = maths::Cross(zHat, Vec3(0, 1, 0));
			yHat = maths::Cross(zHat, xHat);
		}

		inline void SetForwards(const Vec3 &_forwards) {
			zHat = _forwards.Normalised();
			xHat = maths::Cross(zHat, Vec3(0, 1, 0));
			yHat = maths::Cross(zHat, xHat);
		}

		inline void SetFov(const float _fov) {
			tanFov = std::tan(.5 * _fov);
			tanFov2 = 2 * tanFov;
		}

		Ray GenerateRay(const float _u, const float _v) const override {
			const Vec3 p = xHat*(_v*-tanFov2+tanFov) + yHat*(_v*-tanFov2*aspect + tanFov*aspect) + zHat;
			return Ray(origin, (p - origin).Normalised());
		}

	protected:
		Vec3 xHat, yHat, zHat;
		float tanFov, tanFov2;
};