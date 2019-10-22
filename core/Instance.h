#pragma once
#include "TriangleMesh.h"

class InstanceProxy {
	public:
		InstanceProxy(Object *_object) {
			iObject = _object;
		}

		void Commit(const RTCDevice &_device) {
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
		Instance(InstanceProxy *_proxy) {
			proxy = _proxy;
		}

		void Commit(const RTCDevice &_device) override {
			geometry = rtcNewGeometry(_device, RTC_GEOMETRY_TYPE_INSTANCE);
			rtcSetGeometryInstancedScene(geometry, proxy->iScene);
			rtcSetGeometryTransform(geometry, 0, RTC_FORMAT_FLOAT3X4_COLUMN_MAJOR, &xfm[0]);
			rtcRetainGeometry(geometry);
			rtcCommitGeometry(geometry);
		}

	protected:
		InstanceProxy *proxy;

		void ProcessHit(RayHit &_hit, const RTCRayHit &_h) const override {
			proxy->iObject->ProcessHit(_hit, _h);
		}
};