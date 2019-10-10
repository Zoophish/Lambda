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

	protected:
		Assimp::Importer importer;

		inline Affine3 aiMatToAff3(aiMatrix4x4 *_aiMat) const {
			Affine3 aff;
			aff[0] = _aiMat->a1; aff[1] = _aiMat->a2; aff[2] = _aiMat->a3;
			aff[3] = _aiMat->b1; aff[4] = _aiMat->b2; aff[5] = _aiMat->b3;
			aff[6] = _aiMat->c1; aff[7] = _aiMat->c2; aff[8] = _aiMat->c3;
			aff[9] = _aiMat->d1; aff[10] = _aiMat->d2; aff[11] = _aiMat->d3;
			return aff;
		}

		bool LoadMeshes(ResourceManager *_resourceManager) const {
			if (scene->HasMeshes()) {
				for (unsigned i = 0; i < scene->mNumMeshes; ++i) {
					TriangleMesh *mesh = new TriangleMesh();
					mesh->LoadFromImport(*this, i);
					_resourceManager->objectPool.Append(scene->mMeshes[i]->mName.C_Str(), mesh);
				}
			}
			else return false;
		}

		/*
		Must load meshes first.
		*/
		bool LoadTransforms(ResourceManager *_resourceManager) const {
			if (scene->mRootNode && _resourceManager->objectPool.Size() > 0) {
				const size_t s = _resourceManager->objectPool.Size();
				for (size_t i = 0; i < s; ++i) {
					//_resourceManager->objectPool.pool.
				}
			}
		}
};