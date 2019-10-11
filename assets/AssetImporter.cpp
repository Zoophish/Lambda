#include "AssetImporter.h"

bool AssetImporter::Import(const std::string &_path) {
	scene = importer.ReadFile(_path, DEFAULT_IMPORT_FLAGS);
	importer.ApplyPostProcessing(aiProcess_CalcTangentSpace);
	if (!scene) {
		std::cout << importer.GetErrorString();
		return false;
	}
	return true;
}

bool AssetImporter::Import(ResourceManager *_resourceManager, const std::string &_path) {
	scene = importer.ReadFile(_path, DEFAULT_IMPORT_FLAGS);
	if (!scene) { std::cout << importer.GetErrorString(); return false; }
	importer.ApplyPostProcessing(aiProcess_CalcTangentSpace);
	MeshImporter::LoadMeshes(scene, _resourceManager);
	return true;
}