//----	By Sam Warren 2019	----

#pragma once
#include <core/Ray.h>
#include "Aperture.h"

LAMBDA_BEGIN

class Camera {
	public:
		Real aperture, shutterSpeed;
		Vec3 origin;

		Camera(const Vec3 &_origin = Vec3(0, 0, 0), const Real _x = 1, const Real _y = 1);

		/*
			Sets camera look direction to spherical coordinates, _phi and _theta.
		*/
		void SetRotation(const Real _phi, const Real _theta);

		/*
			Sets camera look direction to vector _forwards.
				- _forwards can be non-unit in magnitude.
		*/
		void SetForwards(const Vec3 &_forwards);

		/*
			Sets conical viewing angle, _fov, in radians.
		*/
		void SetFov(const Real _fov);

		/*
			Returns a ray for film-plane coordinates, _u and _v.
		*/
		virtual Ray GenerateRay(const Real _u, const Real _v) const = 0;

	protected:
		Vec3 xHat, yHat, zHat;
		Real aspect, tanFov, tanFov2;
};



class PinholeCamera : public Camera {
	public:
		PinholeCamera(const Vec3 &_origin, const Real _x, const Real _y);

		/*
			Returns a ray for film-plane coordinates, _u and _v.
		*/
		Ray GenerateRay(const Real _u, const Real _v) const override;
};



class ThinLensCamera : public Camera {
	public:
		Aperture *aperture;
		Real focalLength;

		ThinLensCamera(const Vec3 &_origin, const Real _x, const Real _y, const Real _focalLength, Aperture *_aperture);
		
		/*
			Returns a ray for film-plane coordinates, _u and _v.
		*/
		Ray GenerateRay(const Real _u, const Real _v) const override;
};



class SphericalCamera : public Camera {
	public:
		Real offsetPhi, offsetTheta;

		SphericalCamera(const Vec3 &_origin);

		/*
			Returns a ray for film-plane coordinates, _u and _v.
		*/
		Ray GenerateRay(const Real _u, const Real _v) const override;
};

LAMBDA_END