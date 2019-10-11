/*----	By Sam Warren 2019	----
  ----	Allows for the construction and loading of meshes that can be passed into Embree.	----

	Any changes made to the mesh data must be committed to take effect.

	Embree only supports 32-bit floats and 16-byte aligned vertex structures.
	Keep defined if Embree is being used:	*/
#define LAMBDA_GEOMETRY_FORCE_FLOAT32

#pragma once
#include <vector>
//#include <assets/AssetImporter.h>
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

		TriangleMesh() {
			smoothNormals = false;
			hasUVs = false;
		}

		//void LoadFromImport(const aiMesh* _aiMesh, const unsigned _index = 0) {
		//	//if (_ai.scene && _ai.scene->HasMeshes()) {
		//		//const aiMesh *mesh = _ai.scene->mMeshes[_index];
		//
		//		//----	VERTICES	----
		//		verticesSize = _aiMesh->mNumVertices;
		//		trianglesSize = _aiMesh->mNumFaces;
		//
		//		vertices.resize(verticesSize);
		//		for (unsigned i = 0; i < verticesSize; ++i) {
		//			vertices[i].x = _aiMesh->mVertices[i].x;
		//			vertices[i].y = _aiMesh->mVertices[i].y;
		//			vertices[i].z = _aiMesh->mVertices[i].z;
		//		}
		//
		//		//----	TRIANGLES	----
		//		triangles.resize(trianglesSize);
		//		for (unsigned i = 0; i < trianglesSize; ++i) {
		//			triangles[i].v0 = _aiMesh->mFaces[i].mIndices[0];
		//			triangles[i].v1 = _aiMesh->mFaces[i].mIndices[1];
		//			triangles[i].v2 = _aiMesh->mFaces[i].mIndices[2];
		//		}
		//
		//		//----	NORMALS	----
		//		if (_aiMesh->HasNormals()) {
		//			vertexNormals.resize(verticesSize);
		//			for (unsigned i = 0; i < verticesSize; ++i) {
		//				vertexNormals[i].x = _aiMesh->mNormals[i].x;
		//				vertexNormals[i].y = _aiMesh->mNormals[i].y;
		//				vertexNormals[i].z = _aiMesh->mNormals[i].z;
		//			}
		//		}
		//
		//		//----	TANGENTS, BITANGENTS	----
		//		if (_aiMesh->HasTangentsAndBitangents()) {
		//			vertexTangents.resize(verticesSize);
		//			vertexBitangents.resize(verticesSize);
		//			for (unsigned i = 0; i < verticesSize; ++i) {
		//				vertexTangents[i].x = _aiMesh->mTangents[i].x;
		//				vertexTangents[i].y = _aiMesh->mTangents[i].y;
		//				vertexTangents[i].z = _aiMesh->mTangents[i].z;
		//				vertexBitangents[i].x = _aiMesh->mBitangents[i].x;
		//				vertexBitangents[i].y = _aiMesh->mBitangents[i].y;
		//				vertexBitangents[i].z = _aiMesh->mBitangents[i].z;
		//			}
		//		}
		//
		//		//----	TEXTURE COORDINATES	----
		//		if (_aiMesh->HasTextureCoords(0)) {
		//			hasUVs = true;
		//			uvs.resize(verticesSize);
		//			for (unsigned i = 0; i < verticesSize; ++i) {
		//				uvs[i].x = _aiMesh->mTextureCoords[0][i].x;
		//				uvs[i].y = _aiMesh->mTextureCoords[0][i].y;
		//			}
		//		}
		//	//}
		//	//else std::cout << std::endl << "No mesh in imported object.";
		//}

		void Commit(const RTCDevice &_device) override {
			geometry = rtcNewGeometry(_device, RTC_GEOMETRY_TYPE_TRIANGLE);
			rtcSetSharedGeometryBuffer(geometry, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, &vertices[0], 0, sizeof(vec3<float>), verticesSize);
			rtcSetSharedGeometryBuffer(geometry, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, &triangles[0], 0, sizeof(Triangle), trianglesSize);
			rtcRetainGeometry(geometry);
			rtcCommitGeometry(geometry);
		}

		//To avoid duplicate computation, we can optionally fetch the normal in the same function.
		inline void GetTriangleAreaAndNormal(const Triangle *_t, Real *_area, Vec3 *_normal = nullptr) const {
			const Vec3 cross = maths::Cross(vertices[_t->v1] - vertices[_t->v0], vertices[_t->v2] - vertices[_t->v0]);
			*_area = cross.Magnitude() * .5;
			if(_normal) *_normal = cross / (*_area * 2.);
		}

		Real Area() const {
			Real area = 0;
			for (size_t i = 0; i < trianglesSize; ++i) {
				const Vec3 cross = maths::Cross(vertices[triangles[i].v1] - vertices[triangles[i].v0], vertices[triangles[i].v2] - vertices[triangles[i].v0]);
				area += cross.Magnitude() * .5;
			}
			return area;
		}

		inline Vec3 SamplePoint(const Triangle &_triangle, const Vec2 &_u) const {
			const Vec3 v0 = vertices[_triangle.v0];
			const Vec3 v1 = vertices[_triangle.v1];
			const Vec3 v2 = vertices[_triangle.v2];
			return v0 + (v1 - v0) * _u.x + (v2 - v0) * _u.y;
		}

	protected:
		void ProcessHit(RayHit &_hit, const RTCRayHit &_h) const override {
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
					_h.hit.u, _h.hit.v);
				_hit.tangent = maths::BarycentricInterpolation(
					vertexTangents[triangles[_h.hit.primID].v0],
					vertexTangents[triangles[_h.hit.primID].v1],
					vertexTangents[triangles[_h.hit.primID].v2],
					_h.hit.u, _h.hit.v);
				_hit.bitangent = maths::BarycentricInterpolation(
					vertexBitangents[triangles[_h.hit.primID].v0],
					vertexBitangents[triangles[_h.hit.primID].v1],
					vertexBitangents[triangles[_h.hit.primID].v2],
					_h.hit.u, _h.hit.v);
			}
			else {
				//Needs optimisation.
				_hit.normalS = _hit.normalG;
				const Vec3 c1 = maths::Cross(_hit.normalG, Vec3(0, 0, 1));
				const Vec3 c2 = maths::Cross(_hit.normalG, Vec3(0, 1, 0));
				_hit.tangent = (maths::Dot(c1,c1) > maths::Dot(c2,c2) ? c1 : c2);
				_hit.bitangent = maths::Cross(_hit.tangent, _hit.normalG);
			}
		}
};