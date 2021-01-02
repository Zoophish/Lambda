/*----	Sam Warren 2019-2020	----
	Any changes made to the mesh data must be committed to take effect.
*/

#pragma once
#include <vector>
#include "Object.h"

LAMBDA_BEGIN

struct Triangle { unsigned v0, v1, v2; };

class TriangleMesh : public Object {
	public:
		Vec3 *vertices;
		Triangle *triangles;
		Vec3 *vertexNormals;
		Vec3 *vertexTangents;
		Vec2 *textureCoordinates;
		size_t numTriangles;
		size_t numVertices;
		bool smoothNormals;

		TriangleMesh();

		~TriangleMesh();

		/*
			Assigns mesh data. Bitangent handedness stored in the fourth component (Vec3.a) of the vertexTangents.
		*/
		void AssignData(const unsigned _numVertices, const unsigned _numTriangles, Vec3 *_vertices, Triangle *_triangles,
			Vec3 *_vertexNormals = nullptr, Vec3 *_vertexTangents = nullptr,
			bool *_bitangentHandednesses = nullptr, Vec2 *_textureCoordinates = nullptr);

		/*
			Allocate mesh buffers according to _numVertices and _numTriangles.
		*/
		void AllocData(const size_t _numVertices, const size_t _numTriangles);

		/*
			Free mesh data from memory.
		*/
		void FreeData();

		/*
			Commit geometry to Embree.
		*/
		void Commit(const RTCDevice &_device) override;

		/*
			Return local bounds.
		*/
		Bounds GetLocalBounds() const override;

		/*
			Calculates _area and optionally _normal of triangle _t.
			To avoid duplicate computation, can optionally fetch the normal in the same function.
		*/
		inline void GetTriangleAreaAndNormal(const Triangle *_t, Real *_area, Vec3 *_normal = nullptr) const {
			const Vec3 cross = maths::Cross(vertices[_t->v1] - vertices[_t->v0], vertices[_t->v2] - vertices[_t->v0]);
			*_area = cross.Magnitude() * (Real).5;
			if(_normal) *_normal = cross / (*_area * 2.);
		}

		/*
			Returns the area of entire mesh.
		*/
		Real Area() const;

		/*
			Samples a point in _triangle.
		*/
		inline Vec3 SamplePointInTriangle(const Triangle &_triangle, const Vec2 &_u) const {
			const Vec3 &v0 = vertices[_triangle.v0];
			const Vec3 &v1 = vertices[_triangle.v1];
			const Vec3 &v2 = vertices[_triangle.v2];
			const Vec3 point = v0 + (v1 - v0) * _u.x + (v2 - v0) * _u.y;
			return point;
		}

	protected:
		void ProcessHit(const RTCRayHit &_rtcHit, RayHit &_hit) const override;
};

LAMBDA_END