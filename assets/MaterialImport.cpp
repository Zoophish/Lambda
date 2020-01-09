#include "MaterialImport.h"
#include <shading/graph/GraphBxDF.h>
#include <shading/graph/GraphInputs.h>
#include <iostream>

namespace MaterialImport {

	namespace sg = ShaderGraph;

	enum MaterialAttributes {
		MTL_ATTRIB_NONE = 0,
		MTL_ATTRIB_DIFFUSE = BITFLAG(1),
		MTL_ATTRIB_GLOSSY = BITFLAG(2),
		MTL_ATTRIB_EMISSIVE = BITFLAG(3),
		MTL_ATTRIB_ALPHA = BITFLAG(4),
	};

	template<class Format>
	void aiTextureToTexture(const aiTexture *_aiTexture, texture_t<Format> *_texture) {
		static constexpr Real inv256 = 1. / 256.;
		const unsigned w = _aiTexture->mWidth, h = _aiTexture->mHeight;
		for (unsigned y = 0; y < h; ++y) {
			for (unsigned x = 0; x < w; ++x) {
				const Real rgba[4] = {
					(Real)_aiTexture->pcData[y * w + x].r * inv256,
					(Real)_aiTexture->pcData[y * w + x].g * inv256,
					(Real)_aiTexture->pcData[y * w + x].b * inv256,
					(Real)_aiTexture->pcData[y * w + x].a * inv256
				};
				_texture->SetPixelCoord(x, y, Format(&rgba[0]));
			}
		}
	}

	template<aiTextureType texType>
	bool PushTextureStacks(const aiScene *_aiScene, ResourceManager *_resources, ImportMetrics *_metrics) {
		aiString path;
		unsigned i = 0;
		for (; i < _aiScene->mNumMaterials; ++i) {
			if (_aiScene->mMaterials[i]->GetTexture(texType, 0, &path) == aiReturn_SUCCESS) {
				const std::string fullPath = _metrics->path + "\\" + path.C_Str();
				if (!_resources->texturePool.Find(path.C_Str())) {
					Texture *tex = new Texture;
					//Check if dimensions are part of 2^n. Change encoder?
					tex->LoadImageFile(fullPath.c_str());
					_resources->texturePool.Append(path.C_Str(), tex);
					_metrics->AppendMetric(fullPath + " [" + std::to_string(tex->GetWidth()) + "x" + std::to_string(tex->GetHeight()) + "] pushed.");
				}
				else _metrics->AppendMetric(std::string(path.C_Str()) + " already pushed.");
			}
		}
		_metrics->AppendMetric(std::to_string(i) + " textures pushed from stacks.");
		return i > 0;
	}

	bool PushTextures(const aiScene *_aiScene, ResourceManager *_resourceManager, ImportMetrics *_metrics) {
		unsigned numTextures = 0;
		_metrics->AppendMetric("Diffuse stack:");
		if (!PushTextureStacks<aiTextureType_DIFFUSE>(_aiScene, _resourceManager, _metrics)) _metrics->AppendMetric("No textures in diffuse stacks.");
		else numTextures++;
		_metrics->AppendMetric("Displacement stack:");
		if (!PushTextureStacks<aiTextureType_DISPLACEMENT>(_aiScene, _resourceManager, _metrics)) _metrics->AppendMetric("No textures in displacement stacks.");
		else numTextures++;
		_metrics->AppendMetric("Glossiness stack:");
		if (!PushTextureStacks<aiTextureType_SHININESS>(_aiScene, _resourceManager, _metrics)) _metrics->AppendMetric("No textures in glossiness stacks.");
		else numTextures++;
		_metrics->AppendMetric(std::to_string(numTextures) + " total textures pushed.");
		return numTextures > 0;
	}

	inline bool HasTransparency(const aiMaterial *_aiMaterial) {
		if (_aiMaterial->GetTexture(aiTextureType_OPACITY, 0, nullptr)) return true;
		
	}

	inline int GetAttributes(const aiMaterial *_aiMaterial) {
		int attributes = MTL_ATTRIB_NONE;
		attributes += _aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, nullptr) == aiReturn_SUCCESS ? MTL_ATTRIB_DIFFUSE : MTL_ATTRIB_NONE;
		attributes += _aiMaterial->GetTexture(aiTextureType_SHININESS, 0, nullptr) == aiReturn_SUCCESS ? MTL_ATTRIB_GLOSSY : MTL_ATTRIB_NONE;
		attributes += _aiMaterial->GetTexture(aiTextureType_OPACITY, 0, nullptr) == aiReturn_SUCCESS ? MTL_ATTRIB_ALPHA : MTL_ATTRIB_NONE;
		attributes += _aiMaterial->GetTexture(aiTextureType_EMISSIVE, 0, nullptr) == aiReturn_SUCCESS ? MTL_ATTRIB_EMISSIVE : MTL_ATTRIB_NONE;
		return attributes;
	}

	//DEBUG / TEST - Diffuse material with diffuse texture from assimp.
	void aiMaterialToMaterial(const aiMaterial *_aiMaterial, const ResourceManager *_resources, Material *_material, ImportMetrics *_metrics) {
		//const int attributes = GetAttributes(_aiMaterial);
		aiString diffusePath;
		const bool hasDiffuse = _aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &diffusePath) == aiReturn_SUCCESS;
		Texture *tex = nullptr;
		if (hasDiffuse && _resources->texturePool.Find(diffusePath.C_Str(), tex)) {
			sg::ImageTextureInput *img = _material->graphArena.New<sg::ImageTextureInput>(tex);
			sg::ScalarInput *sig = _material->graphArena.New<sg::ScalarInput>(1.5);
			_material->bxdf = _material->graphArena.New<sg::OrenNayarBxDFNode>(&img->outputSockets[0], &sig->outputSockets[0]);
		}
		else {
			sg::ImageTextureInput *img = _material->graphArena.New<sg::ImageTextureInput>(new Texture(1,1, Colour(1,1,0)));
			sg::ScalarInput *sig = _material->graphArena.New<sg::ScalarInput>(1.5);
			_material->bxdf = _material->graphArena.New<sg::OrenNayarBxDFNode>(&img->outputSockets[0], &sig->outputSockets[0]);
			aiString matName; _aiMaterial->Get(AI_MATKEY_NAME, matName);
			_metrics->AppendError("Could not find texture for " + std::string(matName.C_Str()) + ". Debug texture applied instead.");
		}
	}

	bool PushMaterials(const aiScene *_aiScene, ResourceManager *_resources, ImportMetrics *_metrics) {
		if (_aiScene->HasMaterials()) {
			for (unsigned i = 0; i < _aiScene->mNumMaterials; ++i) {
				Material *mat = new Material;
			   
				aiMaterialToMaterial(_aiScene->mMaterials[i], _resources, mat, _metrics);

				aiString matName;
				_aiScene->mMaterials[i]->Get(AI_MATKEY_NAME, matName);
				_resources->materialPool.Append(matName.C_Str(), mat);
				_metrics->AppendMetric(std::string(matName.C_Str()) + " pushed.");
			}
			return true;
		}
		_metrics->AppendError("No materials in asset.");
		return false;
	}

	Material *GetMaterial(const aiScene *_aiScene, const ResourceManager *_resources, const std::string &_name) {
		for (unsigned i = 0; i < _aiScene->mNumMeshes; ++i) {
			if (_name == _aiScene->mMeshes[i]->mName.C_Str()) {
				const unsigned matIndex = _aiScene->mMeshes[i]->mMaterialIndex;
				aiString matName;
				_aiScene->mMaterials[matIndex]->Get(AI_MATKEY_NAME, matName);
				Material *out = nullptr;
				if (_resources->materialPool.Find(matName.C_Str(), out)) {
					return out;
				}
			}
		}
		return nullptr;
	}

}