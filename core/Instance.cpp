#include "Instance.h"

LAMBDA_BEGIN

InstanceProxy::InstanceProxy(Object *_object) {
	iObject = _object;
}

Instance::Instance(InstanceProxy *_proxy) {
	proxy = _proxy;
}

void Instance::Commit(const RTCDevice &_device) {
	geometry = rtcNewGeometry(_device, RTC_GEOMETRY_TYPE_INSTANCE);
	rtcSetGeometryInstancedScene(geometry, proxy->iScene);
	rtcSetGeometryTransform(geometry, 0, RTC_FORMAT_FLOAT3X4_COLUMN_MAJOR, &xfm[0]);
	rtcRetainGeometry(geometry);
	rtcCommitGeometry(geometry);
}

void Instance::ProcessHit(RayHit &_hit, const RTCRayHit &_h) const {
	proxy->iObject->ProcessHit(_hit, _h);
}

LAMBDA_END