//----	By Sam Warren 2019	----
//----	Assimp asset importer interface for importing 3D scenes of various formats.	----
//----	Assimp: http://www.assimp.org/	----

#pragma once
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "MeshImport.h"

#define DEFAULT_IMPORT_FLAGS aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_GenNormals | aiProcess_OptimizeGraph

class AssetImporter {
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

		bool Import(ResourceManager *_resourceManager, const std::string &_path) {
			scene = importer.ReadFile(_path, DEFAULT_IMPORT_FLAGS);
			if (!scene) { std::cout << importer.GetErrorString(); return false; }
			importer.ApplyPostProcessing(aiProcess_CalcTangentSpace);
			MeshImport::LoadMeshes(scene, _resourceManager);
			return true;
		}

	protected:
		Assimp::Importer importer;
};