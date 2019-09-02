#pragma once
#include "Camera.h"
#include "Aperture.h"

class ThinLensCamera : public Camera {
	public:
	Aperture *aperture;
	Real focalLength;

	ThinLensCamera(const Vec3 &_origin, const float _x, const float _y, const Real _focalLength) : Camera(_x, _y) {
		origin = _origin;
		focalLength = _focalLength;
		SetFov(1);
		SetRotation(0, 0);
	}

	inline void SetRotation(const float _phi, const float _theta) {
		zHat = Vec3(std::cos(_theta) * std::sin(_phi), std::sin(_theta), std::cos(_theta) * std::cos(_phi)).Normalised();
		xHat = maths::Cross(zHat, Vec3(0, 1, 0));
		yHat = maths::Cross(xHat, zHat);
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
		const Vec3 fp = origin + (xHat * (_u * -tanFov2 + tanFov) + yHat * (_v * -tanFov2 * aspect + tanFov * aspect) + zHat) * focalLength;
		Real pdf;
		const Vec2 ap = aperture ? aperture->Sample_p(&pdf) : Vec2(0,0);
		const Vec3 o = origin + xHat * ap.x + yHat * ap.y;
		return Ray(o, (fp - o).Normalised());
	}

	protected:
		Vec3 xHat, yHat, zHat;
		Real tanFov, tanFov2;
};