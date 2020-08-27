/*---- Sam Warren 2019	----
 Assimp asset importer interface.
 Assimp: http://www.assimp.org/
 */

#pragma once
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "MeshImport.h"
#include "MaterialImport.h"
#include "GraphImport.h"

LAMBDA_BEGIN

enum ImportOptions : uint8_t {
	IMP_ALL = 15,
	IMP_MESHES = 1,
	IMP_TEXTURES = 2,
	IMP_MATERIALS = 4,
	IMP_GRAPH = 8
};

class AssetImporter {
	public:
		const aiScene* scene;
		
		AssetImporter();

		/*
			Loads and processes asset file, _path, into aiScene.
		*/
		bool Import(const char *_path);

		/*
			Transfers the Assimp data into lambda compatible data in _resources.
		*/
		void PushToResourceManager(ResourceManager *_resources, const ImportOptions _impOpt = IMP_ALL);

		/*
			Release the imported aiScene from memory.
		*/
		void Release();

	protected:
		Assimp::Importer importer;
		std::string path;
};

LAMBDA_END