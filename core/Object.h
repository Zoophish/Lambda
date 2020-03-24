//----	Sam Warren 2019	----
//Embree object base class.

#pragma once
#include <core/Transformable.h>
#include "Ray.h"

LAMBDA_BEGIN

class MediaBoundary;
class Light;
class BxDF;

class Object : public Transformable {
	public:
		RTCGeometry geometry;
		BxDF *bxdf = nullptr;
		Light *light = nullptr;
		MediaBoundary *mediaBoundary;
		
		Object() {}

		Object(const Affine3 &_xfm) : Transformable(_xfm) {}
		
		/*
			Process hit information from Embree RTCRayHit to a Lambda RayHit.
		*/
		inline RayHit Hit(const RTCRayHit &_h) const {
			RayHit hit;
			hit.point.x = _h.ray.org_x + _h.ray.dir_x * _h.ray.tfar;
			hit.point.y = _h.ray.org_y + _h.ray.dir_y * _h.ray.tfar;
			hit.point.z = _h.ray.org_z + _h.ray.dir_z * _h.ray.tfar;
			hit.normalG = Vec3(_h.hit.Ng_x, _h.hit.Ng_y, _h.hit.Ng_z).Normalised();
			ProcessHit(hit, _h);
			return hit;
		}

		/*
			Commit geometry changes to Embree.
		*/
		virtual void Commit(const RTCDevice &_device) = 0;

		/*
			All derivatives must override to provide their own hit information.
		*/
		virtual void ProcessHit(RayHit &_hit, const RTCRayHit &_h) const = 0;
};

class Empty : public Object {
	public:
		Empty(const Affine3 &_xfm) : Object(_xfm) {}

		void Commit(const RTCDevice &_device) override {
			return;
		}

		void ProcessHit(RayHit &_hit, const RTCRayHit &_h) const override {
			return;
		}
};

LAMBDA_END