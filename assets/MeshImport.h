/*----	Sam Warren 2019	----
Transfers Assimp aiMesh data into TriangleMesh.
*/
#pragma once
#include "ImportUtilities.h"
#include <core/TriangleMesh.h>

namespace MeshImport {

	using namespace ImportUtilities;

	/*
		Loads given UV channel, _channel, into _tMesh from the Assimp mesh, _aiMesh.
			- Returns false if texture coordinates don't exist in _aiMesh.
	*/
	bool LoadUVBuffer(const aiMesh *_aiMesh, TriangleMesh *_tMesh, const unsigned _channel = 0);

	/*
		Copies main vertex attributes from the Assimp mesh, _aiMesh, into the Lambda mesh, _tMesh.
		Vertex attributes copied:
			- Position
			- Texture Coordinate
			- Normal
			- Tangent / Bitangent
	*/
	void LoadMeshVertexBuffers(const aiMesh *_aiMesh, TriangleMesh *_tMesh);

	/*
		Loads transform data from aiScene into any corresponding objects in the ResourceManager.
			- Renaming objects from their original in the resource pool will prevent its transform loading.
	*/
	bool LoadTransforms(const aiScene *_scene, ResourceManager *_resourceManager);

	/*
		Loads all meshes in _aiScene into the _resourcemanager object pool.
	*/
	bool PushMeshes(const aiScene *_scene, ResourceManager *_resourceManager, ImportMetrics *_metrics);

}