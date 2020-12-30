#include "MeshImport.h"

LAMBDA_BEGIN

namespace MeshImport {

	inline Real CheckReal(const Real _val) {
		return std::isnan(_val) || std::isinf(_val) ? 0 : _val;
	}

	void LoadMeshVertexBuffers(const aiMesh *_aiMesh, TriangleMesh *_tMesh) {

		_tMesh->AllocData(_aiMesh->mNumVertices, _aiMesh->mNumFaces);

		//	Vertices
		for (size_t i = 0; i < _tMesh->numVertices; ++i) {
			_tMesh->vertices[i].x = _aiMesh->mVertices[i].x;
			_tMesh->vertices[i].y = _aiMesh->mVertices[i].y;
			_tMesh->vertices[i].z = _aiMesh->mVertices[i].z;
		}

		//	Triangles
		for (size_t i = 0; i < _tMesh->numTriangles; ++i) {
			_tMesh->triangles[i].v0 = _aiMesh->mFaces[i].mIndices[0];
			_tMesh->triangles[i].v1 = _aiMesh->mFaces[i].mIndices[1];
			_tMesh->triangles[i].v2 = _aiMesh->mFaces[i].mIndices[2];
		}

		//	Normals
		if (_aiMesh->HasNormals()) {
			for (size_t i = 0; i < _tMesh->numVertices; ++i) {
				_tMesh->vertexNormals[i].x = _aiMesh->mNormals[i].x;
				_tMesh->vertexNormals[i].y = _aiMesh->mNormals[i].y;
				_tMesh->vertexNormals[i].z = _aiMesh->mNormals[i].z;
			}
		}

		//	Tangents and Bitangent Handednessses
		if (_aiMesh->HasTangentsAndBitangents()) {
			for (size_t i = 0; i < _tMesh->numVertices; ++i) {
				_tMesh->vertexTangents[i].x = CheckReal(_aiMesh->mTangents[i].x);
				_tMesh->vertexTangents[i].y = CheckReal(_aiMesh->mTangents[i].y);
				_tMesh->vertexTangents[i].z = CheckReal(_aiMesh->mTangents[i].z);
				
				const Vec3 bitangent = Vec3(CheckReal(_aiMesh->mBitangents[i].x), CheckReal(_aiMesh->mBitangents[i].y), CheckReal(_aiMesh->mBitangents[i].z));
				const Real dot = maths::Dot(maths::Cross(_tMesh->vertexNormals[i], _tMesh->vertexTangents[i]), bitangent);
				_tMesh->vertexTangents[i].a = (dot < (Real)0) ? (Real)-1 : (Real)1;
			}
		}
		_tMesh->smoothNormals = _aiMesh->HasTangentsAndBitangents();

		//	Texture Coordinates
		static const int channel = 0;
		if (_aiMesh->HasTextureCoords(channel)) {
			_tMesh->hasUVs = true;
			for (size_t i = 0; i < _tMesh->numVertices; ++i) {
				_tMesh->textureCoordinates[i].x = (Real)1 - _aiMesh->mTextureCoords[channel][i].x;
				_tMesh->textureCoordinates[i].y = (Real)1 - _aiMesh->mTextureCoords[channel][i].y;
			}
		}
	}

	bool LoadTransforms(const aiScene *_scene, ResourceManager *_resourceManager) {
		if (_scene->mRootNode && _resourceManager->objectPool.Size() > 0) {
			for (auto &pair : _resourceManager->objectPool.pool) {
				const aiNode *node = _scene->mRootNode->FindNode(pair.first.c_str());
				pair.second->xfm = aiMatToAff3(node->mTransformation);
			}
			return true;
		}
		return false;
	}

	bool PushMeshes(const aiScene *_scene, ResourceManager *_resourceManager, ImportMetrics *_metrics) {
		if (_scene->HasMeshes()) {
			for (unsigned i = 0; i < _scene->mNumMeshes; ++i) {
				TriangleMesh *mesh = new TriangleMesh();
				LoadMeshVertexBuffers(_scene->mMeshes[i], mesh);
				_resourceManager->objectPool.Append(_scene->mMeshes[i]->mName.C_Str(), mesh);
				_metrics->AppendMetric(std::string(_scene->mMeshes[i]->mName.C_Str()) + ":	" +
					std::to_string(_scene->mMeshes[i]->mNumVertices) + " verts, " +
					std::to_string(_scene->mMeshes[i]->mNumFaces) + " tris");
			}
			_metrics->AppendMetric(std::to_string(_scene->mNumMeshes) + " meshes pushed.");
			return !_metrics->HasErrors();
		}
		_metrics->AppendError("Asset has no meshes.");
		return false;
	}

}

LAMBDA_END