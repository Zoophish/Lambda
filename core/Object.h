//----	Sam Warren 2019	----
//Embree object base class.

#pragma once
#include <core/Transformable.h>
#include <shading/Material.h>
#include "Ray.h"

LAMBDA_BEGIN

class MediaBoundary;
class Light;
class BxDF;

class Object : public Transformable {
	public:
		RTCGeometry geometry;
		//BxDF *bxdf = nullptr;
		//Light *light = nullptr;
		//MediaBoundary *mediaBoundary;
		Material *material;
		
		Object() {}

		Object(const Affine3 &_xfm) : Transformable(_xfm) {}
		
		/*
			Process hit information from Embree RTCRayHit to a Lambda RayHit.
		*/
		inline void Hit(const RTCRayHit &_rtcHit, RayHit &_hit) const {
			_hit.tFar = _rtcHit.ray.tfar;
			_hit.point.x = _rtcHit.ray.org_x + _rtcHit.ray.dir_x * _rtcHit.ray.tfar;
			_hit.point.y = _rtcHit.ray.org_y + _rtcHit.ray.dir_y * _rtcHit.ray.tfar;
			_hit.point.z = _rtcHit.ray.org_z + _rtcHit.ray.dir_z * _rtcHit.ray.tfar;
			_hit.normalG = Vec3(_rtcHit.hit.Ng_x, _rtcHit.hit.Ng_y, _rtcHit.hit.Ng_z).Normalised();
			ProcessHit(_rtcHit, _hit);
		}

		/*
			Commit geometry changes to Embree.
		*/
		virtual void Commit(const RTCDevice &_device) = 0;

		/*
			All derivatives must override to provide their own hit information.
		*/
		virtual void ProcessHit(const RTCRayHit &_rtcHit, RayHit &_hit) const = 0;
};

class Empty : public Object {
	public:
		Empty(const Affine3 &_xfm) : Object(_xfm) {}

		void Commit(const RTCDevice &_device) override {
			return;
		}

		void ProcessHit(const RTCRayHit &_rtcHit, RayHit &_hit) const override {
			return;
		}
};

LAMBDA_END