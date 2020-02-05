#include "AssetImporter.h"

LAMBDA_BEGIN

AssetImporter::AssetImporter() {
	scene = nullptr;
}

bool AssetImporter::Import(const char *_path) {
	path = _path;
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

void AssetImporter::PushToResourceManager(ResourceManager *_resources, const ImportOptions _impOpt) {
	if (scene) {
		if (_impOpt & IMP_MESHES) {
			ImportUtilities::ImportMetrics meshImportMetrics(std::string(path.c_str()) + " meshes", path);
			if (!MeshImport::PushMeshes(scene, _resources, &meshImportMetrics)) std::cout << std::endl << "Mesh import failed.";
			//meshImportMetrics.LogAll();
		}
		if (_impOpt & IMP_TEXTURES) {
			ImportUtilities::ImportMetrics textureImportMetrics(std::string(path.c_str()) + " textures", path);
			if (!MaterialImport::PushTextures(scene, _resources, &textureImportMetrics)) std::cout << std::endl << "Texture import failed.";
			textureImportMetrics.LogAll();
		}
		if (_impOpt & IMP_MATERIALS) {
			ImportUtilities::ImportMetrics materialImportMetrics(std::string(path.c_str()) + " materials", path);
			if (!MaterialImport::PushMaterials(scene, _resources, &materialImportMetrics)) std::cout << std::endl << "Material import failed.";
			materialImportMetrics.LogAll();
		}
		if (_impOpt & IMP_GRAPH) {
			ImportUtilities::ImportMetrics graphImportMetrics(std::string(path.c_str()) + " graph", path);
			if (!GraphImport::PushGraph(scene, _resources, &graphImportMetrics)) std::cout << std::endl << "Graph import failed.";
			//graphImportMetrics.LogAll();
		}

	}
	else std::cout << std::endl << "Can't transfer to resource manager: No content imported.";
}

void AssetImporter::Release() {
	scene->~aiScene();
	scene = nullptr;
	path.clear();
}

LAMBDA_END