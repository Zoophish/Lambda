#include "AssetImporter.h"

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
			ImportUtilities::ImportMetrics meshImportMetrics(scene->GetShortFilename(path.c_str()));
			if (MeshImport::PushMeshes(scene, _resources, &meshImportMetrics))
				meshImportMetrics.LogMetrics();
			else meshImportMetrics.LogErrors();
		}
		if (_impOpt & IMP_TEXTURES) {
			ImportUtilities::ImportMetrics textureImportMetrics(scene->GetShortFilename(path.c_str()));
			if (MaterialImport::PushTextures(scene, _resources, &textureImportMetrics))
				textureImportMetrics.LogMetrics();
			else textureImportMetrics.LogErrors();
		}
		if (_impOpt & IMP_MATERIALS) {

		}
		if (_impOpt & IMP_GRAPH) {
			ImportUtilities::ImportMetrics graphImportMetrics(scene->GetShortFilename(path.c_str()));
			if (GraphImport::PushGraph(scene, _resources, &graphImportMetrics))
				graphImportMetrics.LogMetrics();
			else graphImportMetrics.LogErrors();
		}

	}
	else std::cout << std::endl << "Can't transfer to resource manager: No content imported.";
}

void AssetImporter::Release() {
	scene->~aiScene();
	scene = nullptr;
	path.clear();
}