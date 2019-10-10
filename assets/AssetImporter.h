//----	By Sam Warren 2019	----
//----	Assimp asset importer interface for importing 3D scenes of various formats.	----
//----	Assimp: http://www.assimp.org/	----

#pragma once
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ResourceManager.h"

#define DEFAULT_IMPORT_FLAGS aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GenSmoothNormals | aiProcess_OptimizeGraph

class AssetImporter {
	protected:
		Assimp::Importer importer;

	public:
		const aiScene* scene;
		
		AssetImporter() {
			scene = nullptr;
		}

		bool Import(const std::string &_path) {
			scene = importer.ReadFile(_path, DEFAULT_IMPORT_FLAGS);
			importer.ApplyPostProcessing(aiProcess_CalcTangentSpace);
			if (!scene) {
				std::cout << importer.GetErrorString();
				return false;
			}
			return true;
		}

		bool Import(const std::string &_path, ResourceManager *_resourceManager) {
			scene = importer.ReadFile(_path, DEFAULT_IMPORT_FLAGS);
			importer.ApplyPostProcessing(aiProcess_CalcTangentSpace);
			if (!scene) {
				std::cout << importer.GetErrorString();
				return false;
			}
			return true;
		}

		//protected:

			//bool LoadMeshes(ResourceManager *_resourceManager) const {
			//	if (scene->HasMeshes()) {
			//		for (unsigned i = 0; i < scene->mNumMeshes; ++i) {
			//			TriangleMesh *mesh = new TriangleMesh();
			//			//mesh->LoadFromImport()
			//		}
			//	}
			//	else return false;
			//}
};