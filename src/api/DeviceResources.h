#pragma once
#include <vector>

#include "..\core\TriangleMesh.h"
#include "..\core\Instance.h"
#include "..\image\Texture.h"
#include "..\lighting\EnvironmentLight.h"
#include "..\lighting\MeshLight.h"
#include "..\lighting\PointLight.h"
#include "..\lighting\Spotlight.h"


using namespace lambda;

struct DeviceResources {
	std::vector<TriangleMesh> triangleMeshes;
	std::vector<Instance> instances;
	std::vector<Texture> textures;
	std::vector<EnvironmentLight> environmentLights;
	std::vector<MeshLight> meshLights;
	std::vector<PointLight> pointLights;
	std::vector<Spotlight> spotLights;
};