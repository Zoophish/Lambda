#include "TriangleMesh.h"

LAMBDA_BEGIN

TriangleMesh::TriangleMesh() : Object() {
	vertices = nullptr;
	vertexNormals = nullptr;
	vertexTangents = nullptr;
	textureCoordinates = nullptr;
	triangles = nullptr;
	numVertices = 0;
	numTriangles = 0;
	smoothNormals = false;
	hasUVs = false;
}

TriangleMesh::~TriangleMesh() {
	FreeData();
}

void TriangleMesh::AssignData(const unsigned _numVertices, const unsigned _numTriangles, Vec3 *_vertices, Triangle *_triangles,
	Vec3 *_vertexNormals, Vec3 *_vertexTangents, bool *_bitangentHandednesses, Vec2 *_textureCoordinates) {
	numVertices = _numVertices;
	numTriangles = _numTriangles;
	vertices = _vertices;
	triangles = _triangles;
	vertexNormals = _vertexNormals;
	vertexTangents = _vertexTangents;
	textureCoordinates = _textureCoordinates;
}

void TriangleMesh::AllocData(const size_t _numVertices, const size_t _numTriangles) {
	FreeData();
	numVertices = _numVertices;
	numTriangles = _numTriangles;
	vertices = new Vec3[numVertices];
	triangles = new Triangle[numTriangles];
	vertexNormals = new Vec3[numVertices];
	vertexTangents = new Vec3[numVertices];
	textureCoordinates = new Vec2[numVertices];
}

void TriangleMesh::FreeData() {
	if(vertices) delete[] vertices;
	if(vertexNormals) delete[] vertexNormals;
	if(vertexTangents) delete[] vertexTangents;
	if(textureCoordinates) delete[] textureCoordinates;
	if(triangles) delete[] triangles;
	vertices = nullptr;
	vertexNormals = nullptr;
	vertexTangents = nullptr;
	textureCoordinates = nullptr;
	triangles = nullptr;
	numVertices = 0;
	numTriangles = 0;
}

void TriangleMesh::Commit(const RTCDevice &_device) {
	geometry = rtcNewGeometry(_device, RTC_GEOMETRY_TYPE_TRIANGLE);
	rtcSetSharedGeometryBuffer(geometry, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, &vertices[0], 0, sizeof(Vec3), numVertices);
	rtcSetSharedGeometryBuffer(geometry, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, &triangles[0], 0, sizeof(Triangle), numTriangles);
	rtcRetainGeometry(geometry);
	rtcCommitGeometry(geometry);
}

Bounds TriangleMesh::GetLocalBounds() const {
	Bounds bounds;
	for (size_t i = 0; i < numVertices; ++i) bounds = maths::Union(bounds, vertices[i]);
	return bounds;
}

Real TriangleMesh::Area() const {
	Real area = 0;
	for (size_t i = 0; i < numTriangles; ++i) {
		const Vec3 cross = maths::Cross(vertices[triangles[i].v1] - vertices[triangles[i].v0], vertices[triangles[i].v2] - vertices[triangles[i].v0]);
		area += cross.Magnitude();
	}
	return area * (Real).5;
}

void TriangleMesh::ProcessHit(const RTCRayHit &_h, RayHit &_hit) const {
	if (hasUVs) {
		_hit.uvCoords = maths::BarycentricInterpolation(
			textureCoordinates[triangles[_h.hit.primID].v0],
			textureCoordinates[triangles[_h.hit.primID].v1],
			textureCoordinates[triangles[_h.hit.primID].v2],
			_h.hit.u, _h.hit.v);
	}
	if (smoothNormals) {
		const Vec3 &n0 = vertexNormals[triangles[_h.hit.primID].v0];
		const Vec3 &n1 = vertexNormals[triangles[_h.hit.primID].v1];
		const Vec3 &n2 = vertexNormals[triangles[_h.hit.primID].v2];
		const Vec3 &t0 = vertexTangents[triangles[_h.hit.primID].v0];
		const Vec3 &t1 = vertexTangents[triangles[_h.hit.primID].v1];
		const Vec3 &t2 = vertexTangents[triangles[_h.hit.primID].v2];

		_hit.normalS = maths::BarycentricInterpolation(n0, n1, n2, _h.hit.u, _h.hit.v).Normalised();

		_hit.tangent = maths::BarycentricInterpolation(t0, t1, t2, _h.hit.u, _h.hit.v).Normalised();

		_hit.bitangent = maths::BarycentricInterpolation(
			maths::Cross(n0, t0) * t0.a,
			maths::Cross(n1, t1) * t1.a,
			maths::Cross(n2, t2) * t2.a,
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