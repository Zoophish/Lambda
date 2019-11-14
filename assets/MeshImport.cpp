#include "MeshImport.h"

namespace MeshImport {

	bool LoadUVBuffer(const aiMesh *_aiMesh, TriangleMesh *_tMesh, const unsigned _channel) {
		if (_aiMesh->HasTextureCoords(_channel)) {
			_tMesh->hasUVs = true;
			_tMesh->uvs.resize(_tMesh->verticesSize);
			for (size_t i = 0; i < _tMesh->verticesSize; ++i) {
				_tMesh->uvs[i].x = _aiMesh->mTextureCoords[_channel][i].x;
				_tMesh->uvs[i].y = _aiMesh->mTextureCoords[_channel][i].y;
			}
			return true;
		}
		return false;
	}

	void LoadMeshVertexBuffers(const aiMesh *_aiMesh, TriangleMesh *_tMesh) {
		//----	VERTICES	----
		_tMesh->verticesSize = _aiMesh->mNumVertices;
		_tMesh->trianglesSize = _aiMesh->mNumFaces;

		_tMesh->vertices.resize(_tMesh->verticesSize);
		for (size_t i = 0; i < _tMesh->verticesSize; ++i) {
			_tMesh->vertices[i].x = _aiMesh->mVertices[i].x;
			_tMesh->vertices[i].y = _aiMesh->mVertices[i].y;
			_tMesh->vertices[i].z = _aiMesh->mVertices[i].z;
		}

		//----	TRIANGLES	----
		_tMesh->triangles.resize(_tMesh->trianglesSize);
		for (size_t i = 0; i < _tMesh->trianglesSize; ++i) {
			_tMesh->triangles[i].v0 = _aiMesh->mFaces[i].mIndices[0];
			_tMesh->triangles[i].v1 = _aiMesh->mFaces[i].mIndices[1];
			_tMesh->triangles[i].v2 = _aiMesh->mFaces[i].mIndices[2];
		}

		//----	NORMALS	----
		if (_aiMesh->HasNormals()) {
			_tMesh->vertexNormals.resize(_tMesh->verticesSize);
			for (size_t i = 0; i < _tMesh->verticesSize; ++i) {
				_tMesh->vertexNormals[i].x = _aiMesh->mNormals[i].x;
				_tMesh->vertexNormals[i].y = _aiMesh->mNormals[i].y;
				_tMesh->vertexNormals[i].z = _aiMesh->mNormals[i].z;
			}
		}

		//----	TANGENTS, BITANGENTS	----
		if (_aiMesh->HasTangentsAndBitangents()) {
			_tMesh->vertexTangents.resize(_tMesh->verticesSize);
			_tMesh->vertexBitangents.resize(_tMesh->verticesSize);
			for (size_t i = 0; i < _tMesh->verticesSize; ++i) {
				_tMesh->vertexTangents[i].x = _aiMesh->mTangents[i].x;
				_tMesh->vertexTangents[i].y = _aiMesh->mTangents[i].y;
				_tMesh->vertexTangents[i].z = _aiMesh->mTangents[i].z;
				_tMesh->vertexBitangents[i].x = _aiMesh->mBitangents[i].x;
				_tMesh->vertexBitangents[i].y = _aiMesh->mBitangents[i].y;
				_tMesh->vertexBitangents[i].z = _aiMesh->mBitangents[i].z;
			}
		}
		_tMesh->smoothNormals = _aiMesh->HasTangentsAndBitangents();

		//----	TEXTURE COORDINATES	----
		_tMesh->hasUVs = LoadUVBuffer(_aiMesh, _tMesh);
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