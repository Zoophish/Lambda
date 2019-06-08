#include <iostream>
#include <core/TriangleMesh.h>
#include <core/Scene.h>

int main() {

	AssetImporter imp;
	imp.Import("monkey.blend");

	std::cout << imp.scene->mMeshes[0]->mNumVertices << std::endl;

	system("pause");
	
	return 0;
}