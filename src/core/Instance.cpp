#include "Instance.h"

LAMBDA_BEGIN

InstanceProxy::InstanceProxy() {}

InstanceProxy::InstanceProxy(Object *_object) {
	iObject = _object;
}

void InstanceProxy::Commit(const RTCDevice &_device) {
	iScene = rtcNewScene(_device);
	iObject->Commit(_device);
	rtcAttachGeometry(iScene, iObject->geometry);
	rtcCommitScene(iScene);
}

Instance::Instance() : Object() {}

Instance::Instance(InstanceProxy *_proxy) : Object() {
	proxy = _proxy;
}

void Instance::Commit(const RTCDevice &_device) {
	if (proxy) {
		if (!material) material = proxy->iObject->material;
		geometry = rtcNewGeometry(_device, RTC_GEOMETRY_TYPE_INSTANCE);
		rtcSetGeometryInstancedScene(geometry, proxy->iScene);
		Affine3 worldXfm = GetAffine();
		rtcSetGeometryTransform(geometry, 0, RTC_FORMAT_FLOAT3X4_COLUMN_MAJOR, &worldXfm[0]);
		rtcRetainGeometry(geometry);
		rtcCommitGeometry(geometry);
	}
}

void Instance::ProcessHit(const RTCRayHit &_rtcHit, RayHit &_hit) const {
	proxy->iObject->ProcessHit(_rtcHit, _hit);
	if (!xfm.IsIdentity()) {
		TransformNormal(&_hit.normalS);
		TransformNormal(&_hit.tangent);
		TransformNormal(&_hit.bitangent);
	}
}

LAMBDA_END