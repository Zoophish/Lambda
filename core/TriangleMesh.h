/*----	By Sam Warren 2019	----
  ----	Allows for the construction and loading of meshes that can be passed into Embree.	----

	Any changes made to the mesh data must be committed to take effect.

	Embree only supports 32-bit floats and 16-byte aligned vertex structures.
	Keep defined if Embree is being used:	*/
#define LAMBDA_GEOMETRY_FORCE_FLOAT32

#pragma once
#include <vector>
#include "Object.h"

struct Triangle { unsigned v0, v1, v2; };

class TriangleMesh : public Object {
	public:
		#ifdef LAMBDA_GEOMETRY_FORCE_FLOAT32
			std::vector<vec3<float>> vertices;
		#else
			std::vector<Vec3> vertices;
		#endif
		std::vector<Vec3> vertexNormals, vertexTangents, vertexBitangents;
		std::vector<Triangle> triangles;
		std::vector<Vec2> uvs;
		size_t trianglesSize, verticesSize;
		bool smoothNormals, hasUVs;

		TriangleMesh();

		void Commit(const RTCDevice &_device) override;

		//To avoid duplicate computation, we can optionally fetch the normal in the same function.
		inline void GetTriangleAreaAndNormal(const Triangle *_t, Real *_area, Vec3 *_normal = nullptr) const {
			const Vec3 cross = maths::Cross(vertices[_t->v1] - vertices[_t->v0], vertices[_t->v2] - vertices[_t->v0]);
			*_area = cross.Magnitude() * .5;
			if(_normal) *_normal = cross / (*_area * 2.);
		}

		Real Area() const;

		inline Vec3 SamplePoint(const Triangle &_triangle, const Vec2 &_u) const {
			const Vec3 &v0 = vertices[_triangle.v0];
			const Vec3 &v1 = vertices[_triangle.v1];
			const Vec3 &v2 = vertices[_triangle.v2];
			return v0 + (v1 - v0) * _u.x + (v2 - v0) * _u.y;
		}

	protected:
		void ProcessHit(RayHit &_hit, const RTCRayHit &_h) const override;
};