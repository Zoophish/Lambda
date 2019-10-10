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
		}

		void Commit(const RTCDevice &_device) override {
			geometry = rtcNewGeometry(*proxy->iDevice, RTC_GEOMETRY_TYPE_INSTANCE);
			rtcSetGeometryInstancedScene(geometry, proxy->iScene);
			rtcRetainGeometry(geometry);
			rtcSetGeometryTransform(geometry, 0, RTC_FORMAT_FLOAT3X4_COLUMN_MAJOR, &xfm[0]);
			rtcCommitGeometry(geometry);
		}

	protected:
		InstanceProxy *proxy;

		void ProcessHit(RayHit &_hit, const RTCRayHit &_h) const override {
			proxy->iObject->ProcessHit(_hit, _h);
		}
};