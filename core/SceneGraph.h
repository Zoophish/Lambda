#pragma once
#include <string>
#include "Scene.h"

LAMBDA_BEGIN

struct SceneNode {
	std::string name;
	std::vector<SceneNode> children;
	std::vector<Object*> objects;
	Affine3 transform;
};

LAMBDA_END