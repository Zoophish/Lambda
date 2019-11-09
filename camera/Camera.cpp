#include "Camera.h"

//----------------CAMERA----------------

Camera::Camera(const Vec3 &_origin, const Real _x, const Real _y) {
	origin = _origin;
	SetFov(1.1);
	SetRotation(0, 0);
}

void Camera::SetRotation(const Real _phi, const Real _theta) {
	zHat = Vec3(std::cos(_theta) * std::sin(_phi), std::sin(_theta), std::cos(_theta) * std::cos(_phi)).Normalised();
	xHat = maths::Cross(zHat, Vec3(0, 1, 0));
	yHat = maths::Cross(xHat, zHat);
}

void Camera::SetForwards(const Vec3 &_forwards) {
	zHat = _forwards.Normalised();
	xHat = maths::Cross(zHat, Vec3(0, 1, 0));
	yHat = maths::Cross(zHat, xHat);
}

void Camera::SetFov(const Real _fov) {
	tanFov = std::tan(.5 * _fov);
	tanFov2 = 2 * tanFov;
}

//---------------- Pinhole Camera ----------------

PinholeCamera::PinholeCamera(const Vec3 &_origin, const Real _x, const Real _y) : Camera(_origin, _x, _y) {}

Ray PinholeCamera::GenerateRay(const Real _u, const Real _v) const {
	const Vec3 p = origin + xHat * (_u * -tanFov2 + tanFov) + yHat * (_v * -tanFov2 * aspect + tanFov * aspect) + zHat;
	return Ray(origin, (p - origin).Normalised());
}

//---------------- Thin Lens Camera ----------------

ThinLensCamera::ThinLensCamera(const Vec3 &_origin, const Real _x, const Real _y, const Real _focalLength, Aperture *_aperture) : Camera(_origin, _x, _y) {
	focalLength = _focalLength;
	aperture = _aperture;
}

Ray ThinLensCamera::GenerateRay(const Real _u, const Real _v) const {
	const Vec3 fp = origin + (xHat * (_u * -tanFov2 + tanFov) + yHat * (_v * -tanFov2 * aspect + tanFov * aspect) + zHat) * focalLength;
	Real pdf;
	const Vec2 ap = aperture ? aperture->Sample_p(&pdf) : Vec2(0, 0);
	const Vec3 o = origin + xHat * ap.x + yHat * ap.y;
	return Ray(o, (fp - o).Normalised());
}

//---------------- Spherical Camera ----------------

SphericalCamera::SphericalCamera(const Vec3 &_origin) : Camera(_origin) {
	origin = _origin;
}

Ray SphericalCamera::GenerateRay(const Real _u, const Real _v) const {
	const Real phi = PI2 * _u + offsetPhi;
	const Real theta = PI * _v + offsetTheta;
	const Vec3 d = maths::SphericalDirection(std::sin(theta), std::cos(theta), phi).Normalised();
	return Ray(origin, xHat * d.x + yHat * d.y + zHat * d.z);
}