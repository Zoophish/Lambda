#pragma once
#include "ImportUtilities.h"
#include <image/Texture.h>
#include <shading/Material.h>

namespace MaterialImport {

	using namespace ImportUtilities;

	/*
		Pushes all textures referenced in _aiScene into _resources.
	*/
	bool PushTextures(const aiScene *_aiScene, ResourceManager *_resource, ImportMetrics *_metrics);

	/*
		Pushes all materials referenced in _aiScene into _resources.
	*/
	bool PushMaterials(const aiScene *_aiScene, ResourceManager *_resources, ImportMetrics *_metrics);

	/*
		Returns the corresponding material in _resources to the given object _name from _aiScene.
	*/
	Material *GetMaterial(const aiScene *_aiScene, const ResourceManager *_resources, const std::string &_name);
}