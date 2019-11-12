/*----	Sam Warren 2019	----
Transfers Assimp aiMesh data into TriangleMesh.
*/
#pragma once
#include <assimp/scene.h>
#include <core/TriangleMesh.h>
#include "ResourceManager.h"

namespace MeshImport {

	/*
		Loads given UV channel, _channel, into _tMesh from the Assimp mesh, _aiMesh.
			- Returns false if texture coordinates don't exist in _aiMesh.
	*/
	bool LoadUVBuffer(const aiMesh *_aiMesh, TriangleMesh *_tMesh, const unsigned _channel = 0);

	/*
		Essentially copies main vertex attributes from the Assimp mesh, _aiMesh, into the Lambda mesh, _tMesh.
		Vertex attributes copied:
			- Position
			- Texture Coordinate
			- Normal
			- Tangent / Bitangent
	*/
	void LoadMeshVertexBuffers(const aiMesh *_aiMesh, TriangleMesh *_tMesh);

	/*
		Converts aiMatrix4x4 to Affine3.
	*/
	inline Affine3 aiMatToAff3(const aiMatrix4x4 &_aiMat) {
		Affine3 aff;
		aff[0] = _aiMat.a1; aff[1] = _aiMat.a2; aff[2] = _aiMat.a3;
		aff[3] = _aiMat.b1; aff[4] = _aiMat.b2; aff[5] = _aiMat.b3;
		aff[6] = _aiMat.c1; aff[7] = _aiMat.c2; aff[8] = _aiMat.c3;
		aff[9] = _aiMat.d1; aff[10] = _aiMat.d2; aff[11] = _aiMat.d3;
		return aff;
	}

	/*
		Loads transform data from aiScene into any corresponding objects in the ResourceManager.
			- Renaming objects from their original in the resource pool will prevent its transform loading.
	*/
	bool LoadTransforms(const aiScene *_scene, ResourceManager *_resourceManager);

	/*
		Loads all meshes in _aiScene into the _resourcemanager object pool.
	*/
	bool LoadMeshes(const aiScene *_scene, ResourceManager *_resourceManager);

}