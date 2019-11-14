#include "MaterialImport.h"

namespace MaterialImport {

	bool PushTextures(const aiScene *_aiScene, ResourceManager *_resourceManager, ImportMetrics *_metrics) {
		if (_aiScene->HasTextures()) {
			for (unsigned i = 0; i < _aiScene->mNumTextures; ++i) {
				Texture *tex = new Texture();
				const std::string fileName = _aiScene->mTextures[i]->mFilename.C_Str();
				tex->LoadImageFile(fileName.c_str()); //Maybe misunderstanding of filename?
				//_aiScene->mTextures[i].
				const std::string texName = _aiScene->GetShortFilename(_aiScene->mTextures[i]->mFilename.C_Str());
				_resourceManager->texturePool.Append(texName, tex);
				_metrics->AppendMetric(texName + " added.");
			}
			_metrics->AppendMetric(std::to_string(_aiScene->mNumTextures) + " textures pushed.");
			return true;
		}
		_metrics->AppendError("No textures in asset.");
		return false;
	}

	bool PushMaterials(const aiScene *_aiScene, ResourceManager *_resources, ImportMetrics *_metrics) {
		if (_aiScene->HasMaterials()) {
			for (unsigned i = 0; i < _aiScene->mNumMaterials; ++i) {
				Material *mat = new Material;
				//_aiScene->mMaterials[0].
			}
		}
		_metrics->AppendError("No materials in asset.");
		return false;
	}

}