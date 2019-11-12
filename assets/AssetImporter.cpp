#include "AssetImporter.h"

AssetImporter::AssetImporter() {
	scene = nullptr;
}

bool AssetImporter::Import(const char *_path) {
	std::cout << std::endl << "Loading " << _path;
	scene = importer.ReadFile(_path, DEFAULT_IMPORT_FLAGS);
	importer.ApplyPostProcessing(aiProcess_CalcTangentSpace);
	if (!scene) {
		std::cout << importer.GetErrorString();
		return false;
	}
	std::cout << std::endl << "Done.";
	return true;
}

bool AssetImporter::Import(ResourceManager *_resourceManager, const char *_path) {
	Import(_path);
	MeshImport::LoadMeshes(scene, _resourceManager);
	return true;
}