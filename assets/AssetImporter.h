//----	By Sam Warren 2019	----
//----	Assimp asset importer interface for importing 3D scenes of various formats.	----
//----	Assimp: http://www.assimp.org/	----

#pragma once
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "MeshImport.h"

#define DEFAULT_IMPORT_FLAGS aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_GenNormals

class AssetImporter {
	public:
		const aiScene* scene;
		
		AssetImporter();

		bool Import(const char *_path);

		bool Import(ResourceManager *_resourceManager, const char *_path);

	protected:
		Assimp::Importer importer;
};