#include "MaterialImport.h"
#include <shading/graph/GraphBxDF.h>
#include <shading/graph/GraphInputs.h>
#include <iostream>

namespace MaterialImport {

	namespace sg = ShaderGraph;

	namespace {

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

		bool HasAlpha(const Texture *_tex) {
			const size_t s = _tex->GetWidth() * _tex->GetHeight();
			for (size_t i = 0; i < s; ++i) {
				const Real a = (*_tex)[i].a;
				if (std::isnormal(a) && a < (Real)1 && a >(Real)0) return true;
			}
			return false;
		}

		inline bool IsPowerOf2(const int _n) {
			return (_n & (_n - 1)) == 0;
		}

		template<aiTextureType texType>
		bool PushTextureStacks(const aiScene *_aiScene, ResourceManager *_resources, ImportMetrics *_metrics) {
			aiString path;
			unsigned i = 0;
			for (; i < _aiScene->mNumMaterials; ++i) {
				if (_aiScene->mMaterials[i]->GetTexture(texType, 0, &path) == aiReturn_SUCCESS) {
					const std::string fullPath = (std::string(path.C_Str()).find(':') != std::string::npos) ? path.C_Str() : _metrics->path + "\\" + path.C_Str();
					if (!_resources->texturePool.Find(path.C_Str())) {
						Texture *tex = new Texture;
						int w, h;
						Texture::GetFileInfo(fullPath.c_str(), &w, &h);
						if (IsPowerOf2(w) && IsPowerOf2(h)) {
							if (w == h) tex->SetEncoder(EncoderMode::ENCODE_HILBERT);
							else tex->SetEncoder(EncoderMode::ENCODE_MORTON);
						}
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

		enum MaterialAttribute {
			MTL_ATTRIB_NONE = 0,
			MTL_ATTRIB_DIFFUSE = BITFLAG(1),
			MTL_ATTRIB_GLOSSY = BITFLAG(2),
			MTL_ATTRIB_EMISSIVE = BITFLAG(3),
			MTL_ATTRIB_ALPHA = BITFLAG(4),
		};

		enum ShadingModel {
			SHADINGMODEL_DIFFUSE = MTL_ATTRIB_DIFFUSE,
			SHADINGMODEL_DIFFUSE_ALPHA = MTL_ATTRIB_DIFFUSE | MTL_ATTRIB_ALPHA,
			SHADINGMODEL_DIFFUSE_EMISSIVE = MTL_ATTRIB_DIFFUSE | MTL_ATTRIB_EMISSIVE,
			SHADINGMODEL_DIFFUSE_GLOSSYCOAT = MTL_ATTRIB_DIFFUSE | MTL_ATTRIB_GLOSSY,
			SHADINGMODEL_EMISSIVE = MTL_ATTRIB_EMISSIVE,
			SHADINGMODEL_EMISSIVE_ALPHA = MTL_ATTRIB_EMISSIVE | MTL_ATTRIB_ALPHA,
			SHADINGMODEL_GLOSSY = MTL_ATTRIB_GLOSSY
		};

		template<aiTextureType texType>
		inline Texture *GetTextureFromPool(const aiMaterial *_aiMaterial, const ResourcePool<Texture> *_texPool) {
			aiString path;
			if (_aiMaterial->GetTexture(texType, 0, &path) == aiReturn_SUCCESS) {
				return _texPool->Find(path.C_Str());
			}
			return nullptr;
		}

		namespace MatMake {

			inline sg::OrenNayarBxDFNode *MakeDiffuse(Material *_material, Texture *_texD) {
				sg::ImageTextureInput *albedo = _material->graphArena.New<sg::ImageTextureInput>(_texD);
				sg::ScalarInput *sig = _material->graphArena.New<sg::ScalarInput>(2);
				return _material->graphArena.New<sg::OrenNayarBxDFNode>(&albedo->outputSockets[0], &sig->outputSockets[0]);
			}

			inline sg::GhostBTDFNode *MakeGhost(Material *_material, const Real _alpha = 1) {
				sg::ScalarInput *scalar = _material->graphArena.New<sg::ScalarInput>(_alpha);
				return _material->graphArena.New<sg::GhostBTDFNode>(&scalar->outputSockets[0]);
			}

			inline sg::MixBxDFNode *MakeDiffuseAlphaMap(Material *_material, Texture *_texD, Texture *_texA) {
				sg::ImageTextureInput *alpha = _material->graphArena.New<sg::ImageTextureInput>(_texA);
				sg::OrenNayarBxDFNode *diffuseBxDF = MakeDiffuse(_material, _texD);
				sg::GhostBTDFNode *ghostBxDF = MakeGhost(_material);
				return _material->graphArena.New<sg::MixBxDFNode>(&ghostBxDF->outputSockets[0], &diffuseBxDF->outputSockets[0], &alpha->outputSockets[1]);
			}

			inline sg::MixBxDFNode *MakeDiffuseAlpha(Material *_material, Texture *_texD) {
				sg::ImageTextureChannelInput *alpha = _material->graphArena.New<sg::ImageTextureChannelInput>(_texD, 3);
				sg::OrenNayarBxDFNode *diffuseBxDF = MakeDiffuse(_material, _texD);
				sg::GhostBTDFNode *ghostBTDF = MakeGhost(_material, 1);
				sg::ScalarInput *scl = _material->graphArena.New<sg::ScalarInput>(.1);
				sg::MixBxDFNode *mixBxDF = _material->graphArena.New<sg::MixBxDFNode>(&diffuseBxDF->outputSockets[0], &ghostBTDF->outputSockets[0], &scl->outputSockets[0]);
				return _material->graphArena.New<sg::MixBxDFNode>(&ghostBTDF->outputSockets[0], &mixBxDF->outputSockets[0], &alpha->outputSockets[0]);
			}

			inline sg::Socket *MakeEmission(Material *_material, Texture *_texE) {
				sg::ImageTextureInput *emissionTex = _material->graphArena.New<sg::ImageTextureInput>(_texE);
				return &emissionTex->outputSockets[0];
			}

			inline sg::LambertianBRDFNode *MakeMissing(Material *_material, Texture *_texD) {
				sg::ImageTextureInput *diffuse = _material->graphArena.New<sg::ImageTextureInput>(_texD);
				return _material->graphArena.New<sg::LambertianBRDFNode>(&diffuse->outputSockets[0]);
			}
		}

		bool BuildMaterial(const int _model, const aiMaterial *_aiMaterial, ResourcePool<Texture> *_texPool, Material *_material) {
			switch (_model) {
			case SHADINGMODEL_DIFFUSE:
			{
				Texture *diffuseTex = GetTextureFromPool<aiTextureType_DIFFUSE>(_aiMaterial, _texPool);
				if (diffuseTex) {
					if (HasAlpha(diffuseTex)) {
						_material->bxdf = MatMake::MakeDiffuseAlpha(_material, diffuseTex);
						return true;
					}
					_material->bxdf = MatMake::MakeDiffuse(_material, diffuseTex);
					return true;
				}
				else goto SHADINGMODEL_MISSING;
			}
			case SHADINGMODEL_DIFFUSE_ALPHA:
			{
				Texture *diffuseTex = GetTextureFromPool<aiTextureType_DIFFUSE>(_aiMaterial, _texPool);
				Texture *alphaTex = GetTextureFromPool<aiTextureType_OPACITY>(_aiMaterial, _texPool);
				if (diffuseTex && alphaTex) {
					_material->bxdf = MatMake::MakeDiffuseAlphaMap(_material, diffuseTex, alphaTex);
					return true;
				}
				else if (HasAlpha(diffuseTex)) {
					_material->bxdf = MatMake::MakeDiffuseAlpha(_material, diffuseTex);
					return true;
				}
				else goto SHADINGMODEL_MISSING;
			}
			//case SHADINGMODEL_EMISSIVE :
			//{
			//	Texture *emissionTex = GetTextureFromPool<aiTextureType_EMISSIVE>(_aiMaterial, _texPool);
			//	const Real intensity = 1;
			//	if (emissionTex) {
			//		_material->light. ShadingModelToMat::MakeEmission(_material, emissionTex);
			//		return true;
			//	}
			//	return false;
			//}
		SHADINGMODEL_MISSING:
			{
				Texture *debugTex = _texPool->Find("DEBUG_TEX");
				if (!debugTex) {
					debugTex = new Texture(1, 1, Colour(1, 1, 0));
					_texPool->Append("DEBUG_TEX", debugTex);
				}
				_material->bxdf = MatMake::MakeMissing(_material, debugTex);
				return false;
			}
			}
		}

		inline int GetMaterialAttributes(const aiMaterial *_aiMaterial) {
			int attributes = MTL_ATTRIB_NONE;
			aiString aiS;
			attributes += _aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiS) == aiReturn_SUCCESS ? MTL_ATTRIB_DIFFUSE : MTL_ATTRIB_NONE;
			attributes += _aiMaterial->GetTexture(aiTextureType_SHININESS, 0, &aiS) == aiReturn_SUCCESS ? MTL_ATTRIB_GLOSSY : MTL_ATTRIB_NONE;
			attributes += _aiMaterial->GetTexture(aiTextureType_OPACITY, 0, &aiS) == aiReturn_SUCCESS ? MTL_ATTRIB_ALPHA : MTL_ATTRIB_NONE;
			attributes += _aiMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &aiS) == aiReturn_SUCCESS ? MTL_ATTRIB_EMISSIVE : MTL_ATTRIB_NONE;
			return attributes;
		}

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

	bool PushMaterials(const aiScene *_aiScene, ResourceManager *_resources, ImportMetrics *_metrics) {
		if (_aiScene->HasMaterials()) {
			for (unsigned i = 0; i < _aiScene->mNumMaterials; ++i) {	
				Material *mat = new Material;
				const int attributes = GetMaterialAttributes(_aiScene->mMaterials[i]);
				const bool pass = BuildMaterial(attributes, _aiScene->mMaterials[i], &_resources->texturePool, mat);
				aiString matName;
				_aiScene->mMaterials[i]->Get(AI_MATKEY_NAME, matName);
				if (!pass) {
					_metrics->AppendError("Failed to build " + (std::string)matName.C_Str() + "'s original material graph.");
				}
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
				return _resources->materialPool.Find(matName.C_Str());
			}
		}
		return nullptr;
	}

}