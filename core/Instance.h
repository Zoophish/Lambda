#pragma once
#include "TriangleMesh.h"

class InstanceProxy {
	public:
		InstanceProxy(Object *_object);

		/*
			Commits iObject to the instance and builds the internal accelleration stucture.
				- Must be called before Instances are committed to a scene.
		*/
		inline void Commit(const RTCDevice &_device) {
			iScene = rtcNewScene(_device);
			iObject->Commit(_device);
			rtcAttachGeometry(iScene, iObject->geometry);
			rtcCommitScene(iScene);
		}

	protected:
		friend class Instance;
		RTCScene iScene;
		Object *iObject;
};

class Instance : public Object {
	public:
		Instance(InstanceProxy *_proxy);

		/*
			Links the proxy and commits the transform.
				- Proxy must be comitted before instance is.
		*/
		void Commit(const RTCDevice &_device) override;

	protected:
		InstanceProxy *proxy;

		/*
			Uses proxy's iObject for hit information.
		*/
		void ProcessHit(RayHit &_hit, const RTCRayHit &_h) const override;
};