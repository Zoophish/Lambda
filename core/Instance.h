#pragma once
#include "TriangleMesh.h"

class InstanceProxy {
	public:
		InstanceProxy(RTCDevice *_device, Object *_object) {
			iScene = rtcNewScene(*_device);
			iDevice = _device;
			iObject = _object;
			rtcCommitGeometry(iObject->geometry);
			rtcAttachGeometry(iScene, iObject->geometry);
			rtcCommitScene(iScene);
		}

	protected:
		friend class Instance;
		RTCScene iScene;
		RTCDevice *iDevice;
		Object *iObject;
};

class Instance : public Object {
	public:
		Instance(InstanceProxy *_proxy) {
			proxy = _proxy;
			geometry = rtcNewGeometry(*proxy->iDevice, RTC_GEOMETRY_TYPE_INSTANCE);
			rtcSetGeometryInstancedScene(geometry, proxy->iScene);
			rtcRetainGeometry(geometry);
		}

	protected:
		InstanceProxy *proxy;

		void ProcessHit(RayHit &_hit, const RTCRayHit &_h) const override {
			proxy->iObject->ProcessHit(_hit, _h);
		}
};