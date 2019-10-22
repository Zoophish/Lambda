#pragma once
#include <assimp/scene.h>
#include <core/Scene.h>
#include "ResourceManager.h"

namespace SceneImport {

	void LoadScene(const aiScene *_aiScene, Scene *_scene) {
		if (_aiScene->mRootNode) {
			
		}
	}

	void LoadNode(const aiNode *_aiNode, const ResourceManager *_resources, Scene *_scene) {
		if (_aiNode->mChildren) {
			for (unsigned i = 0; i < _aiNode->mNumChildren; ++i) {
				
				
			}
		}
	}

}