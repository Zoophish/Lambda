//----	By Sam Warren 2019	----
//----	Assimp asset importer interface for importing 3D scenes of various formats.	----
//----	Assimp: http://www.assimp.org/	----

#pragma once
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "MeshImport.h"

#define DEFAULT_IMPORT_FLAGS aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GenSmoothNormals | aiProcess_OptimizeGraph

class AssetImporter {
	public:
		const aiScene* scene;
		
		AssetImporter() {
			scene = nullptr;
		}

		bool Import(const std::string &_path);

		bool Import(ResourceManager *_resourceManager, const std::string &_path);

	protected:
		Assimp::Importer importer;
};