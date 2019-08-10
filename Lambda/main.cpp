#include <iostream>
#include <core/TriangleMesh.h>
#include <core/Scene.h>
#include <core/Spectrum.h>

int main() {

	AssetImporter imp;
	imp.Import("monkey.blend");
	
	Spectrum test;

	system("pause");
	
	return 0;
}