#include "TriangleMesh.h"

LAMBDA_BEGIN

TriangleMesh::TriangleMesh() {
	smoothNormals = false;
	hasUVs = false;
}

void TriangleMesh::Commit(const RTCDevice &_device) {
	geometry = rtcNewGeometry(_device, RTC_GEOMETRY_TYPE_TRIANGLE);
	rtcSetSharedGeometryBuffer(geometry, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, &vertices[0], 0, sizeof(vec3<float>), verticesSize);
	rtcSetSharedGeometryBuffer(geometry, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, &triangles[0], 0, sizeof(Triangle), trianglesSize);
	rtcRetainGeometry(geometry);
	rtcCommitGeometry(geometry);
}

Real TriangleMesh::Area() const {
	Real area = 0;
	for (size_t i = 0; i < trianglesSize; ++i) {
		const Vec3 cross = maths::Cross(vertices[triangles[i].v1] - vertices[triangles[i].v0], vertices[triangles[i].v2] - vertices[triangles[i].v0]);
		area += cross.Magnitude();
	}
	return area * .5;
}

void TriangleMesh::ProcessHit(const RTCRayHit &_h, RayHit &_hit) const {
	if (hasUVs) {
		_hit.uvCoords = maths::BarycentricInterpolation(
			uvs[triangles[_h.hit.primID].v0],
			uvs[triangles[_h.hit.primID].v1],
			uvs[triangles[_h.hit.primID].v2],
			_h.hit.u, _h.hit.v);
	}
	if (smoothNormals) {
		_hit.normalS = maths::BarycentricInterpolation(
			vertexNormals[triangles[_h.hit.primID].v0],
			vertexNormals[triangles[_h.hit.primID].v1],
			vertexNormals[triangles[_h.hit.primID].v2],
			_h.hit.u, _h.hit.v).Normalised();
		_hit.tangent = maths::BarycentricInterpolation(
			vertexTangents[triangles[_h.hit.primID].v0],
			vertexTangents[triangles[_h.hit.primID].v1],
			vertexTangents[triangles[_h.hit.primID].v2],
			_h.hit.u, _h.hit.v).Normalised();
		_hit.bitangent = maths::BarycentricInterpolation(
			vertexBitangents[triangles[_h.hit.primID].v0],
			vertexBitangents[triangles[_h.hit.primID].v1],
			vertexBitangents[triangles[_h.hit.primID].v2],
			_h.hit.u, _h.hit.v).Normalised();
	}
	else {
		//Needs optimisation.
		_hit.normalS = _hit.normalG;
		const Vec3 c1 = maths::Cross(_hit.normalG, Vec3(0, 0, 1));
		const Vec3 c2 = maths::Cross(_hit.normalG, Vec3(0, 1, 0));
		_hit.tangent = (maths::Dot(c1, c1) > maths::Dot(c2, c2) ? c1 : c2);
		_hit.bitangent = maths::Cross(_hit.tangent, _hit.normalG);
	}
}

LAMBDA_END