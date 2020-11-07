#pragma once
#include "TriangleMesh.h"

LAMBDA_BEGIN

class InstanceProxy {
	public:
		InstanceProxy(Object *_object);

		/*
			Commits iObject to the instance and builds the internal accelleration stucture.
				- Must be called before instances are committed to a scene.
				- Isn't called in Instance otherwise it could be committed many times.
		*/
		void Commit(const RTCDevice &_device);

	protected:
		friend class Instance;
		RTCScene iScene;
		Object *iObject;
};

class Instance : public Object {
	public:
		Instance(InstanceProxy *_proxy);

		/*
			Links to the proxy and commits the transform
				- Proxy MUST be committed before instance is or program will crash
				- Uses proxy's material if not overriden by this instance
		*/
		void Commit(const RTCDevice &_device) override;

	protected:
		InstanceProxy *proxy;

		/*
			Uses proxy's iObject for hit information and transforms it respectively to this instance
		*/
		void ProcessHit(const RTCRayHit &_h, RayHit &_hit) const override;
};

LAMBDA_END