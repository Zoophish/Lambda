/*----	By Sam Warren 2019	----
  ----	Allows for the construction and loading of meshes that can be passed into Embree.	----

	Embree will refer to the data kept here when rays are queried. Any changes made to the
	mesh data must be committed to take effect.

	DEVELOPMENT NOTE: May create a resource manager that stores all imported data, then this
	class only has to point to the data in resource manager; saves memory.
*/

#pragma once
#include <vector>
#include <assets/AssetImporter.h>
#include "Object.h"

#define BITFLAG(_x) 1 << _x

struct Triangle { unsigned v0, v1, v2; };
struct Vertex { float x, y, z, a; };

class TriangleMesh : public Object {
	public:
	friend class TriangleLight;
	bool smoothNormals, hasUVs;

	TriangleMesh() {
		smoothNormals = true;
		hasUVs = false;
	}

	void LoadFromImport(RTCDevice &_device, const AssetImporter &_ai, const unsigned _index = 0) {
		if (_ai.scene && _ai.scene->HasMeshes()) {
			const aiMesh *mesh = _ai.scene->mMeshes[_index];

			//----	VERTICES	----
			const unsigned vertexSize = mesh->mNumVertices;
			const unsigned trianglesSize = mesh->mNumFaces;

			geometry = rtcNewGeometry(_device, RTC_GEOMETRY_TYPE_TRIANGLE);

			vertices = (Vertex *)rtcSetNewGeometryBuffer(geometry, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(Vertex), vertexSize);
			//vertices = std::vector<Vertex>(v, v + vertexSize);
			for (unsigned i = 0; i < vertexSize-1; ++i) {
				vertices[i].x = mesh->mVertices[i].x;
				vertices[i].y = mesh->mVertices[i].y;
				vertices[i].z = mesh->mVertices[i].z;
			}
			//rtcSetSharedGeometryBuffer(geometry, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, vertices, 0, sizeof(Vertex), vertexSize);

			//----	TRIANGLES	----
			triangles = (Triangle *)rtcSetNewGeometryBuffer(geometry, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(Triangle), trianglesSize);
			//triangles = std::vector<Triangle>(t, t + trianglesSize);
			for (unsigned i = 0; i < trianglesSize; ++i) {
				triangles[i].v0 = mesh->mFaces[i].mIndices[0];
				triangles[i].v1 = mesh->mFaces[i].mIndices[1];
				triangles[i].v2 = mesh->mFaces[i].mIndices[2];
			}
			//rtcSetSharedGeometryBuffer(geometry, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, triangles, 0, sizeof(Triangle), trianglesSize);


			//----	NORMALS	----
			if (mesh->HasNormals()) {
				vertexNormals.resize(vertexSize);
				for (unsigned i = 0; i < vertexSize; ++i) {
					vertexNormals[i].x = mesh->mNormals[i].x;
					vertexNormals[i].y = mesh->mNormals[i].y;
					vertexNormals[i].z = mesh->mNormals[i].z;
				}
			}

			//----	TANGENTS, BITANGENTS	----
			if (mesh->HasTangentsAndBitangents()) {
				vertexTangents.resize(vertexSize);
				vertexBitangents.resize(vertexSize);
				for (unsigned i = 0; i < vertexSize; ++i) {
					vertexTangents[i].x = mesh->mTangents[i].x;
					vertexTangents[i].y = mesh->mTangents[i].y;
					vertexTangents[i].z = mesh->mTangents[i].z;
					vertexBitangents[i].x = mesh->mBitangents[i].x;
					vertexBitangents[i].y = mesh->mBitangents[i].y;
					vertexBitangents[i].z = mesh->mBitangents[i].z;
				}
			}

			//----	TEXTURE COORDINATES	----
			if (mesh->HasTextureCoords(0)) {
				hasUVs = true;
				uvs.resize(vertexSize);
				for (unsigned i = 0; i < vertexSize; ++i) {
					uvs[i].x = mesh->mTextureCoords[0][i].x;
					uvs[i].y = mesh->mTextureCoords[0][i].y;
				}
			}
		}
		else std::cout << std::endl << "No mesh in imported object.";
	}

	protected:
	Vertex *vertices;
	Triangle *triangles;
	std::vector<Vec2> uvs;
	std::vector<Vec3> vertexNormals, vertexTangents, vertexBitangents;

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
			_hit.normalS = _hit.normalG;
		}
		_hit.tangent = maths::Cross(_hit.normalS, Vec3(0, 1, 0));
		_hit.bitangent = maths::Cross(_hit.normalS, _hit.tangent);
	}
};