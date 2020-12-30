/*----	Sam Warren 2019	----
Transfers Assimp aiMesh data into TriangleMesh.
*/
#pragma once
#include "ImportUtilities.h"
#include <core/TriangleMesh.h>

LAMBDA_BEGIN

namespace MeshImport {

	using namespace ImportUtilities;

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

LAMBDA_END