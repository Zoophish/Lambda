#pragma once
#include <assimp/scene.h>
#include <image/Texture.h>
#include "ResourceManager.h"

namespace MaterialImport {

	void LoadTextures(const aiScene *_scene, ResourceManager *_resourceManager) {
		if (_scene->HasTextures) {
			for (unsigned i = 0; i < _scene->mNumTextures; ++i) {
				Texture *tex = new Texture();
				tex->LoadImageFile(_scene->mTextures[i]->mFilename.C_Str());
				//_scene->mMaterials[i]->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0)
			}
		}
	}

}