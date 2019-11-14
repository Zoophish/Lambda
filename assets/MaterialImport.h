#pragma once
#include "ImportUtilities.h"
#include <image/Texture.h>
#include <shading/Material.h>

namespace MaterialImport {

	using namespace ImportUtilities;

	/*
		Pushes all textures referenced in _aiScene into _resourceManager.
	*/
	bool PushTextures(const aiScene *_aiScene, ResourceManager *_resource, ImportMetrics *_metrics);

	/*
		
	*/
	bool PushMaterials(const aiScene *_aiScene, ResourceManager *_resources, ImportMetrics *_metrics);
}